
#define _XOPEN_SOURCE 600
#ifndef _GNU_SOURCE
#define _GNU_SOURCE     /* resolves u_char typedef in scsi/scsi.h [lk 2.4] */
#endif

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/file.h>
#include <linux/major.h>
#include <linux/fs.h>   /* <sys/mount.h> */

#include <scsi/scsi.h>
#include <scsi/sg.h>

#include <sg_lib.h>
#include <sg_pt.h>
#include <sg_io_linux.h>

static int sum_of_resids = 0;

static int64_t dd_count = -1;
static int64_t req_count = 0;
static int64_t in_full = 0;
static int in_partial = 0;
static int64_t out_full = 0;
static int out_partial = 0;
static int64_t out_sparse = 0;
static int recovered_errs = 0;
static int unrecovered_errs = 0;
static int read_longs = 0;
static int num_retries = 0;

static int do_time = 0;
static int verbose = 0;
static int start_tm_valid = 0;
static struct timeval start_tm;
static int blk_sz = 0;
static int max_uas = MAX_UNIT_ATTENTIONS;
static int max_aborted = MAX_ABORTED_CMDS;
static int coe_limit = 0;
static int coe_count = 0;

static unsigned char * zeros_buff = NULL;
static int read_long_blk_inc = READ_LONG_DEF_BLK_INC;

static const char * proc_allow_dio = "/proc/scsi/sg/allow_dio";


static struct flags_t iflag;
static struct flags_t oflag;

static void calc_duration_throughput(int contin);

extern "C" int
sg_cmds_process_resp(struct sg_pt_base * ptvp, const char * leadin,
                     int pt_res, int mx_di_len, const unsigned char * sbp,
                     int noisy, int verbose, int * o_sense_cat);


int
sg_err_category_new(int scsi_status, int host_status, int driver_status,
                    const unsigned char * sense_buffer, int sb_len)
{
    int masked_driver_status = (SG_LIB_DRIVER_MASK & driver_status);

    scsi_status &= 0x7e;
    if ((0 == scsi_status) && (0 == host_status) &&
        (0 == masked_driver_status))
        return SG_LIB_CAT_CLEAN;
    if ((SAM_STAT_CHECK_CONDITION == scsi_status) ||
        (SAM_STAT_COMMAND_TERMINATED == scsi_status) ||
        (SG_LIB_DRIVER_SENSE == masked_driver_status))
        return sg_err_category_sense(sense_buffer, sb_len);
    if (0 != host_status) {
        if ((SG_LIB_DID_NO_CONNECT == host_status) ||
            (SG_LIB_DID_BUS_BUSY == host_status) ||
            (SG_LIB_DID_TIME_OUT == host_status))
            return SG_LIB_CAT_TIMEOUT;
    }
    if (SG_LIB_DRIVER_TIMEOUT == masked_driver_status)
        return SG_LIB_CAT_TIMEOUT;
    return SG_LIB_CAT_OTHER;
}

int
sg_err_category3(struct sg_io_hdr * hp)
{
    return sg_err_category_new(hp->status, hp->host_status,
                               hp->driver_status, hp->sbp, hp->sb_len_wr);
}

static const char * linux_driver_bytes[] = {
    "DRIVER_OK", "DRIVER_BUSY", "DRIVER_SOFT", "DRIVER_MEDIA",
    "DRIVER_ERROR", "DRIVER_INVALID", "DRIVER_TIMEOUT", "DRIVER_HARD",
    "DRIVER_SENSE"
};

#define LINUX_DRIVER_BYTES_SZ \
    (int)(sizeof(linux_driver_bytes) / sizeof(linux_driver_bytes[0]))

static const char * linux_driver_suggests[] = {
    "SUGGEST_OK", "SUGGEST_RETRY", "SUGGEST_ABORT", "SUGGEST_REMAP",
    "SUGGEST_DIE", "UNKNOWN","UNKNOWN","UNKNOWN",
    "SUGGEST_SENSE"
};

#define LINUX_DRIVER_SUGGESTS_SZ \
    (int)(sizeof(linux_driver_suggests) / sizeof(linux_driver_suggests[0]))

//FILE *sg_warnings_strm;

void
sg_print_driver_status(int driver_status)
{
    int driv, sugg;
    const char * driv_cp = "invalid";
    const char * sugg_cp = "invalid";

    driv = driver_status & SG_LIB_DRIVER_MASK;
    if (driv < LINUX_DRIVER_BYTES_SZ)
        driv_cp = linux_driver_bytes[driv];
    sugg = (driver_status & SG_LIB_SUGGEST_MASK) >> 4;
    if (sugg < LINUX_DRIVER_SUGGESTS_SZ)
        sugg_cp = linux_driver_suggests[sugg];
    if (NULL == sg_warnings_strm)
        sg_warnings_strm = stderr;
    fprintf(sg_warnings_strm, "Driver_status=0x%02x", driver_status);
    fprintf(sg_warnings_strm, " [%s, %s] ", driv_cp, sugg_cp);
}

static const char * linux_host_bytes[] = {
    "DID_OK", "DID_NO_CONNECT", "DID_BUS_BUSY", "DID_TIME_OUT",
    "DID_BAD_TARGET", "DID_ABORT", "DID_PARITY", "DID_ERROR",
    "DID_RESET", "DID_BAD_INTR", "DID_PASSTHROUGH", "DID_SOFT_ERROR",
    "DID_IMM_RETRY", "DID_REQUEUE", "DID_TRANSPORT_DISRUPTED",
    "DID_TRANSPORT_FAILFAST", "DID_TARGET_FAILURE", "DID_NEXUS_FAILURE",
};

#define LINUX_HOST_BYTES_SZ \
        (int)(sizeof(linux_host_bytes) / sizeof(linux_host_bytes[0]))

void
sg_print_host_status(int host_status)
{
    if (NULL == sg_warnings_strm)
        sg_warnings_strm = stderr;
    fprintf(sg_warnings_strm, "Host_status=0x%02x ", host_status);
    if ((host_status < 0) || (host_status >= LINUX_HOST_BYTES_SZ))
        fprintf(sg_warnings_strm, "is invalid ");
    else
        fprintf(sg_warnings_strm, "[%s] ", linux_host_bytes[host_status]);
}

/* Returns 1 if no errors found and thus nothing printed; otherwise
   prints error/warning (prefix by 'leadin') and returns 0. */
static int
sg_linux_sense_print(const char * leadin, int scsi_status, int host_status,
                     int driver_status, const unsigned char * sense_buffer,
                     int sb_len, int raw_sinfo)
{
    int done_leadin = 0;
    int done_sense = 0;

    if (NULL == sg_warnings_strm)
        sg_warnings_strm = stderr;
    scsi_status &= 0x7e; /*sanity */
    if ((0 == scsi_status) && (0 == host_status) && (0 == driver_status))
        return 1;       /* No problems */
    if (0 != scsi_status) {
        if (leadin)
            fprintf(sg_warnings_strm, "%s: ", leadin);
        done_leadin = 1;
        fprintf(sg_warnings_strm, "SCSI status: ");
        sg_print_scsi_status(scsi_status);
        fprintf(sg_warnings_strm, "\n");
        if (sense_buffer && ((scsi_status == SAM_STAT_CHECK_CONDITION) ||
                             (scsi_status == SAM_STAT_COMMAND_TERMINATED))) {
            /* SAM_STAT_COMMAND_TERMINATED is obsolete */
	    /* XXX
            sg_print_sense(0, sense_buffer, sb_len, raw_sinfo);
	    */
            done_sense = 1;
        }
    }
    if (0 != host_status) {
        if (leadin && (! done_leadin))
            fprintf(sg_warnings_strm, "%s: ", leadin);
        if (done_leadin)
            fprintf(sg_warnings_strm, "plus...: ");
        else
            done_leadin = 1;
        sg_print_host_status(host_status);
        fprintf(sg_warnings_strm, "\n");
    }
    if (0 != driver_status) {
        if (done_sense &&
            (SG_LIB_DRIVER_SENSE == (SG_LIB_DRIVER_MASK & driver_status)))
            return 0;
        if (leadin && (! done_leadin))
            fprintf(sg_warnings_strm, "%s: ", leadin);
        if (done_leadin)
            fprintf(sg_warnings_strm, "plus...: ");
        else
            done_leadin = 1;
        sg_print_driver_status(driver_status);
        fprintf(sg_warnings_strm, "\n");
        if (sense_buffer && (! done_sense) &&
            (SG_LIB_DRIVER_SENSE == (SG_LIB_DRIVER_MASK & driver_status))) {
	    /* XXX
            sg_print_sense(0, sense_buffer, sb_len, raw_sinfo);
	    */
	  }
    }
    return 0;
}

/* Returns 1 if no errors found and thus nothing printed; otherwise
   returns 0. */
int
sg_chk_n_print3(const char * leadin, struct sg_io_hdr * hp,
                int raw_sinfo)
{
    return sg_linux_sense_print(leadin, hp->status, hp->host_status,
                                hp->driver_status, hp->sbp, hp->sb_len_wr,
                                raw_sinfo);
}


static int
sg_build_scsi_cdb(unsigned char * cdbp, int cdb_sz, unsigned int blocks,
                  int64_t start_block, int write_true, int fua, int dpo)
{
#define ME "wipeit: "
    int rd_opcode[] = {0x8, 0x28, 0xa8, 0x88};
    int wr_opcode[] = {0xa, 0x2a, 0xaa, 0x8a};
    int sz_ind;

    memset(cdbp, 0, cdb_sz);
    if (dpo)
        cdbp[1] |= 0x10;
    if (fua)
        cdbp[1] |= 0x8;
    switch (cdb_sz) {
    case 6:
        sz_ind = 0;
        cdbp[0] = (unsigned char)(write_true ? wr_opcode[sz_ind] :
                                               rd_opcode[sz_ind]);
        cdbp[1] = (unsigned char)((start_block >> 16) & 0x1f);
        cdbp[2] = (unsigned char)((start_block >> 8) & 0xff);
        cdbp[3] = (unsigned char)(start_block & 0xff);
        cdbp[4] = (256 == blocks) ? 0 : (unsigned char)blocks;
        if (blocks > 256) {
            fprintf(stderr, ME "for 6 byte commands, maximum number of "
                            "blocks is 256\n");
            return 1;
        }
        if ((start_block + blocks - 1) & (~0x1fffff)) {
            fprintf(stderr, ME "for 6 byte commands, can't address blocks"
                            " beyond %d\n", 0x1fffff);
            return 1;
        }
        if (dpo || fua) {
            fprintf(stderr, ME "for 6 byte commands, neither dpo nor fua"
                            " bits supported\n");
            return 1;
        }
        break;
    case 10:
        sz_ind = 1;
        cdbp[0] = (unsigned char)(write_true ? wr_opcode[sz_ind] :
                                               rd_opcode[sz_ind]);
        cdbp[2] = (unsigned char)((start_block >> 24) & 0xff);
        cdbp[3] = (unsigned char)((start_block >> 16) & 0xff);
        cdbp[4] = (unsigned char)((start_block >> 8) & 0xff);
        cdbp[5] = (unsigned char)(start_block & 0xff);
        cdbp[7] = (unsigned char)((blocks >> 8) & 0xff);
        cdbp[8] = (unsigned char)(blocks & 0xff);
        if (blocks & (~0xffff)) {
            fprintf(stderr, ME "for 10 byte commands, maximum number of "
                            "blocks is %d\n", 0xffff);
            return 1;
        }
        break;
    case 12:
        sz_ind = 2;
        cdbp[0] = (unsigned char)(write_true ? wr_opcode[sz_ind] :
                                               rd_opcode[sz_ind]);
        cdbp[2] = (unsigned char)((start_block >> 24) & 0xff);
        cdbp[3] = (unsigned char)((start_block >> 16) & 0xff);
        cdbp[4] = (unsigned char)((start_block >> 8) & 0xff);
        cdbp[5] = (unsigned char)(start_block & 0xff);
        cdbp[6] = (unsigned char)((blocks >> 24) & 0xff);
        cdbp[7] = (unsigned char)((blocks >> 16) & 0xff);
        cdbp[8] = (unsigned char)((blocks >> 8) & 0xff);
        cdbp[9] = (unsigned char)(blocks & 0xff);
        break;
    case 16:
        sz_ind = 3;
        cdbp[0] = (unsigned char)(write_true ? wr_opcode[sz_ind] :
                                               rd_opcode[sz_ind]);
        cdbp[2] = (unsigned char)((start_block >> 56) & 0xff);
        cdbp[3] = (unsigned char)((start_block >> 48) & 0xff);
        cdbp[4] = (unsigned char)((start_block >> 40) & 0xff);
        cdbp[5] = (unsigned char)((start_block >> 32) & 0xff);
        cdbp[6] = (unsigned char)((start_block >> 24) & 0xff);
        cdbp[7] = (unsigned char)((start_block >> 16) & 0xff);
        cdbp[8] = (unsigned char)((start_block >> 8) & 0xff);
        cdbp[9] = (unsigned char)(start_block & 0xff);
        cdbp[10] = (unsigned char)((blocks >> 24) & 0xff);
        cdbp[11] = (unsigned char)((blocks >> 16) & 0xff);
        cdbp[12] = (unsigned char)((blocks >> 8) & 0xff);
        cdbp[13] = (unsigned char)(blocks & 0xff);
        break;
    default:
        fprintf(stderr, ME "expected cdb size of 6, 10, 12, or 16 but got"
                        " %d\n", cdb_sz);
        return 1;
    }
    return 0;
}

/* 0 -> successful, SG_LIB_SYNTAX_ERROR -> unable to build cdb,
   SG_LIB_CAT_UNIT_ATTENTION -> try again,
   SG_LIB_CAT_MEDIUM_HARD_WITH_INFO -> 'io_addrp' written to,
   SG_LIB_CAT_MEDIUM_HARD -> no info field,
   SG_LIB_CAT_NOT_READY, SG_LIB_CAT_ABORTED_COMMAND,
   -2 -> ENOMEM
   -1 other errors */
static int
sg_read_low(int sg_fd, unsigned char * buff, int blocks, int64_t from_block,
            int bs, const struct flags_t * ifp, int * diop,
            uint64_t * io_addrp)
{
    unsigned char rdCmd[MAX_SCSI_CDBSZ];
    unsigned char senseBuff[SENSE_BUFF_LEN];
    const unsigned char * sbp;
    struct sg_io_hdr io_hdr;
    int res, k, info_valid, slen;

    if (sg_build_scsi_cdb(rdCmd, ifp->cdbsz, blocks, from_block, 0,
                          ifp->fua, ifp->dpo)) {
        fprintf(stderr, ME "bad rd cdb build, from_block=%" PRId64
                ", blocks=%d\n", from_block, blocks);
        return SG_LIB_SYNTAX_ERROR;
    }

    memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = ifp->cdbsz;
    io_hdr.cmdp = rdCmd;
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
    io_hdr.dxfer_len = bs * blocks;
    io_hdr.dxferp = buff;
    io_hdr.mx_sb_len = SENSE_BUFF_LEN;
    io_hdr.sbp = senseBuff;
    io_hdr.timeout = DEF_TIMEOUT;
    io_hdr.pack_id = (int)from_block;
    if (diop && *diop)
        io_hdr.flags |= SG_FLAG_DIRECT_IO;

    if (verbose > 2) {
        fprintf(stderr, "    read cdb: ");
        for (k = 0; k < ifp->cdbsz; ++k)
            fprintf(stderr, "%02x ", rdCmd[k]);
        fprintf(stderr, "\n");
    }
    while (((res = ioctl(sg_fd, SG_IO, &io_hdr)) < 0) && (EINTR == errno))
        ;
    if (res < 0) {
        if (ENOMEM == errno)
            return -2;
        perror("reading (SG_IO) on sg device, error");
        return -1;
    }
    if (verbose > 2)
        fprintf(stderr, "      duration=%u ms\n", io_hdr.duration);
    res = sg_err_category3(&io_hdr);
    sbp = io_hdr.sbp;
    slen = io_hdr.sb_len_wr;
    switch (res) {
    case SG_LIB_CAT_CLEAN:
        break;
    case SG_LIB_CAT_RECOVERED:
        ++recovered_errs;
        info_valid = sg_get_sense_info_fld(sbp, slen, io_addrp);
        if (info_valid) {
            fprintf(stderr, "    lba of last recovered error in this "
                    "READ=0x%" PRIx64 "\n", *io_addrp);
            if (verbose > 1)
                sg_chk_n_print3("reading", &io_hdr, 1);
        } else {
            fprintf(stderr, "Recovered error: [no info] reading from "
                    "block=0x%" PRIx64 ", num=%d\n", from_block, blocks);
            sg_chk_n_print3("reading", &io_hdr, verbose > 1);
        }
        break;
    case SG_LIB_CAT_ABORTED_COMMAND:
    case SG_LIB_CAT_UNIT_ATTENTION:
        sg_chk_n_print3("reading", &io_hdr, verbose > 1);
        return res;
    case SG_LIB_CAT_MEDIUM_HARD:
        if (verbose > 1)
            sg_chk_n_print3("reading", &io_hdr, verbose > 1);
        ++unrecovered_errs;
        info_valid = sg_get_sense_info_fld(sbp, slen, io_addrp);
        /* MMC devices don't necessarily set VALID bit */
        if ((info_valid) || ((5 == ifp->pdt) && (*io_addrp > 0)))
            return SG_LIB_CAT_MEDIUM_HARD_WITH_INFO;
        else {
            fprintf(stderr, "Medium, hardware or blank check error but "
                    "no lba of failure in sense\n");
            return res;
        }
        break;
    case SG_LIB_CAT_NOT_READY:
        ++unrecovered_errs;
        if (verbose > 0)
            sg_chk_n_print3("reading", &io_hdr, verbose > 1);
        return res;
    case SG_LIB_CAT_ILLEGAL_REQ:
        if (5 == ifp->pdt) {    /* MMC READs can go down this path */
            struct sg_scsi_sense_hdr ssh;
            int ili;

            if (verbose > 1)
                sg_chk_n_print3("reading", &io_hdr, verbose > 1);
            if (sg_scsi_normalize_sense(sbp, slen, &ssh) &&
                (0x64 == ssh.asc) && (0x0 == ssh.ascq)) {
                if (sg_get_sense_filemark_eom_ili(sbp, slen, NULL, NULL,
                                                  &ili) && ili) {
                    info_valid = sg_get_sense_info_fld(sbp, slen, io_addrp);
                    if (*io_addrp > 0) {
                        ++unrecovered_errs;
                        return SG_LIB_CAT_MEDIUM_HARD_WITH_INFO;
                    } else
                        fprintf(stderr, "MMC READ gave 'illegal mode for "
                                "this track' and ILI but no LBA of failure\n");
                }
                ++unrecovered_errs;
                return SG_LIB_CAT_MEDIUM_HARD;
            }
        }
        /* drop through */
    default:
        ++unrecovered_errs;
        if (verbose > 0)
            sg_chk_n_print3("reading", &io_hdr, verbose > 1);
        return res;
    }
    if (diop && *diop &&
        ((io_hdr.info & SG_INFO_DIRECT_IO_MASK) != SG_INFO_DIRECT_IO))
        *diop = 0;      /* flag that dio not done (completely) */
    sum_of_resids += io_hdr.resid;
    return 0;
}


/* 0 -> successful, SG_LIB_SYNTAX_ERROR -> unable to build cdb,
   SG_LIB_CAT_UNIT_ATTENTION -> try again, SG_LIB_CAT_NOT_READY,
   SG_LIB_CAT_MEDIUM_HARD, SG_LIB_CAT_ABORTED_COMMAND,
   -2 -> ENOMEM, -1 other errors */
int
sg_read(int sg_fd, unsigned char * buff, int blocks, int64_t from_block,
        int bs, struct flags_t * ifp, int * diop, int * blks_readp)
{
    uint64_t io_addr;
    int64_t lba;
    int res, blks, repeat, xferred;
    unsigned char * bp;
    int retries_tmp;
    int ret = 0;
    int may_coe = 0;

    retries_tmp = ifp->retries;
    for (xferred = 0, blks = blocks, lba = from_block, bp = buff;
         blks > 0; blks = blocks - xferred) {
        io_addr = 0;
        repeat = 0;
        may_coe = 0;
        res = sg_read_low(sg_fd, bp, blks, lba, bs, ifp, diop, &io_addr);
        switch (res) {
        case 0:
            if (blks_readp)
                *blks_readp = xferred + blks;
            if (coe_limit > 0)
                coe_count = 0;  /* good read clears coe_count */
            return 0;
        case -2:        /* ENOMEM */
            return res;
        case SG_LIB_CAT_NOT_READY:
            fprintf(stderr, "Device (r) not ready\n");
            return res;
        case SG_LIB_CAT_ABORTED_COMMAND:
            if (--max_aborted > 0) {
                fprintf(stderr, "Aborted command, continuing (r)\n");
                repeat = 1;
            } else {
                fprintf(stderr, "Aborted command, too many (r)\n");
                return res;
            }
            break;
        case SG_LIB_CAT_UNIT_ATTENTION:
            if (--max_uas > 0) {
                fprintf(stderr, "Unit attention, continuing (r)\n");
                repeat = 1;
            } else {
                fprintf(stderr, "Unit attention, too many (r)\n");
                return res;
            }
            break;
        case SG_LIB_CAT_MEDIUM_HARD_WITH_INFO:
            if (retries_tmp > 0) {
                fprintf(stderr, ">>> retrying a sgio read, lba=0x%" PRIx64
                        "\n", (uint64_t)lba);
                --retries_tmp;
                ++num_retries;
                if (unrecovered_errs > 0)
                    --unrecovered_errs;
                repeat = 1;
            }
            ret = SG_LIB_CAT_MEDIUM_HARD;
            break; /* unrecovered read error at lba=io_addr */
        case SG_LIB_SYNTAX_ERROR:
            ifp->coe = 0;
            ret = res;
            goto err_out;
        case -1:
            ret = res;
            goto err_out;
        case SG_LIB_CAT_MEDIUM_HARD:
            may_coe = 1;
        default:
            if (retries_tmp > 0) {
                fprintf(stderr, ">>> retrying a sgio read, lba=0x%" PRIx64
                        "\n", (uint64_t)lba);
                --retries_tmp;
                ++num_retries;
                if (unrecovered_errs > 0)
                    --unrecovered_errs;
                repeat = 1;
                break;
            }
            ret = res;
            goto err_out;
        }
        if (repeat)
            continue;
        if ((io_addr < (uint64_t)lba) ||
            (io_addr >= (uint64_t)(lba + blks))) {
                fprintf(stderr, "  Unrecovered error lba 0x%" PRIx64 " not "
                        "in correct range:\n\t[0x%" PRIx64 ",0x%" PRIx64
                        "]\n", io_addr, (uint64_t)lba,
                        (uint64_t)(lba + blks - 1));
            may_coe = 1;
            goto err_out;
        }
        blks = (int)(io_addr - (uint64_t)lba);
        if (blks > 0) {
            if (verbose)
                fprintf(stderr, "  partial read of %d blocks prior to "
                        "medium error\n", blks);
            res = sg_read_low(sg_fd, bp, blks, lba, bs, ifp, diop, &io_addr);
            switch (res) {
            case 0:
                break;
            case -1:
                ifp->coe = 0;
                ret = res;
                goto err_out;
            case -2:
                fprintf(stderr, "ENOMEM again, unexpected (r)\n");
                return -1;
            case SG_LIB_CAT_NOT_READY:
                fprintf(stderr, "device (r) not ready\n");
                return res;
            case SG_LIB_CAT_UNIT_ATTENTION:
                fprintf(stderr, "Unit attention, unexpected (r)\n");
                return res;
            case SG_LIB_CAT_ABORTED_COMMAND:
                fprintf(stderr, "Aborted command, unexpected (r)\n");
                return res;
            case SG_LIB_CAT_MEDIUM_HARD_WITH_INFO:
            case SG_LIB_CAT_MEDIUM_HARD:
                ret = SG_LIB_CAT_MEDIUM_HARD;
                goto err_out;
            case SG_LIB_SYNTAX_ERROR:
            default:
                fprintf(stderr, ">> unexpected result=%d from "
                        "sg_read_low() 2\n", res);
                ret = res;
                goto err_out;
            }
        }
        xferred += blks;
        if (0 == ifp->coe) {
            /* give up at block before problem unless 'coe' */
            if (blks_readp)
                *blks_readp = xferred;
            return ret;
        }
        if (bs < 32) {
            fprintf(stderr, ">> bs=%d too small for read_long\n", bs);
            return -1;  /* nah, block size can't be that small */
        }
        bp += (blks * bs);
        lba += blks;
        if ((0 != ifp->pdt) || (ifp->coe < 2)) {
            fprintf(stderr, ">> unrecovered read error at blk=%" PRId64 ", "
                    "pdt=%d, use zeros\n", lba, ifp->pdt);
            memset(bp, 0, bs);
        } else if (io_addr < UINT_MAX) {
            unsigned char * buffp;
            int offset, nl, r, ok, corrct;

            buffp = (unsigned char*)malloc(bs * 2);
            if (NULL == buffp) {
                fprintf(stderr, ">> heap problems\n");
                return -1;
            }
#if 0
            corrct = (ifp->coe > 2) ? 1 : 0;
            res = sg_ll_read_long10(sg_fd, /* pblock */0, corrct, lba, buffp,
                                    bs + read_long_blk_inc, &offset, 1,
                                    verbose);
            ok = 0;
            switch (res) {
            case 0:
                ok = 1;
                ++read_longs;
                break;
            case SG_LIB_CAT_ILLEGAL_REQ_WITH_INFO:
                nl = bs + read_long_blk_inc - offset;
                if ((nl < 32) || (nl > (bs * 2))) {
                    fprintf(stderr, ">> read_long(10) len=%d unexpected\n",
                            nl);
                    break;
                }
                /* remember for next read_long attempt, if required */
                read_long_blk_inc = nl - bs;

                if (verbose)
                    fprintf(stderr, "read_long(10): adjusted len=%d\n", nl);
                r = sg_ll_read_long10(sg_fd, 0, corrct, lba, buffp, nl,
                                      &offset, 1, verbose);
                if (0 == r) {
                    ok = 1;
                    ++read_longs;
                    break;
                } else
                    fprintf(stderr, ">> unexpected result=%d on second "
                            "read_long(10)\n", r);
                break;
            case SG_LIB_CAT_INVALID_OP:
                fprintf(stderr, ">> read_long(10); not supported\n");
                break;
            case SG_LIB_CAT_ILLEGAL_REQ:
                fprintf(stderr, ">> read_long(10): bad cdb field\n");
                break;
            case SG_LIB_CAT_NOT_READY:
                fprintf(stderr, ">> read_long(10): device not ready\n");
                break;
            case SG_LIB_CAT_UNIT_ATTENTION:
                fprintf(stderr, ">> read_long(10): unit attention\n");
                break;
            case SG_LIB_CAT_ABORTED_COMMAND:
                fprintf(stderr, ">> read_long(10): aborted command\n");
                break;
            default:
                fprintf(stderr, ">> read_long(10): problem (%d)\n", res);
                break;
            }
            if (ok)
                memcpy(bp, buffp, bs);
            else
                memset(bp, 0, bs);
            free(buffp);
#endif
		return -1; //XXX
        } else {
            fprintf(stderr, ">> read_long(10) cannot handle blk=%" PRId64
                    ", use zeros\n", lba);
            memset(bp, 0, bs);
        }
        ++xferred;
        bp += bs;
        ++lba;
        if ((coe_limit > 0) && (++coe_count > coe_limit)) {
            if (blks_readp)
                *blks_readp = xferred + blks;
            fprintf(stderr, ">> coe_limit on consecutive reads exceeded\n");
            return SG_LIB_CAT_MEDIUM_HARD;
        }
    }
    if (blks_readp)
        *blks_readp = xferred;
    return 0;

err_out:
    if (ifp->coe) {
        memset(bp, 0, bs * blks);
        fprintf(stderr, ">> unable to read at blk=%" PRId64 " for "
                "%d bytes, use zeros\n", lba, bs * blks);
        if (blks > 1)
            fprintf(stderr, ">>   try reducing bpt to limit number "
                    "of zeros written near bad block(s)\n");
        /* fudge success */
        if (blks_readp)
            *blks_readp = xferred + blks;
        if ((coe_limit > 0) && (++coe_count > coe_limit)) {
            fprintf(stderr, ">> coe_limit on consecutive reads exceeded\n");
            return ret;
        }
        return may_coe ? 0 : ret;
    } else
        return ret ? ret : -1;
}

/* 0 -> successful, SG_LIB_SYNTAX_ERROR -> unable to build cdb,
   SG_LIB_CAT_NOT_READY, SG_LIB_CAT_UNIT_ATTENTION, SG_LIB_CAT_MEDIUM_HARD,
   SG_LIB_CAT_ABORTED_COMMAND, -2 -> recoverable (ENOMEM),
   -1 -> unrecoverable error + others */
int
sg_write(int sg_fd, unsigned char * buff, int blocks, int64_t to_block,
         int bs, const struct flags_t * ofp, int * diop)
{
    unsigned char wrCmd[MAX_SCSI_CDBSZ];
    unsigned char senseBuff[SENSE_BUFF_LEN];
    struct sg_io_hdr io_hdr;
    int res, k, info_valid;
    uint64_t io_addr = 0;

    if (sg_build_scsi_cdb(wrCmd, ofp->cdbsz, blocks, to_block, 1, ofp->fua,
                          ofp->dpo)) {
        fprintf(stderr, ME "bad wr cdb build, to_block=%" PRId64
                ", blocks=%d\n", to_block, blocks);
        return -1;
    }

    memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = ofp->cdbsz;
    io_hdr.cmdp = wrCmd;
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
    io_hdr.dxfer_len = bs * blocks;
    io_hdr.dxferp = buff;
    io_hdr.mx_sb_len = SENSE_BUFF_LEN;
    io_hdr.sbp = senseBuff;
    io_hdr.timeout = DEF_TIMEOUT;
    io_hdr.pack_id = (int)to_block;
    if (diop && *diop)
        io_hdr.flags |= SG_FLAG_DIRECT_IO;

    if (verbose > 2) {
        fprintf(stderr, "    write cdb: ");
        for (k = 0; k < ofp->cdbsz; ++k)
            fprintf(stderr, "%02x ", wrCmd[k]);
        fprintf(stderr, "\n");
    }
    while (((res = ioctl(sg_fd, SG_IO, &io_hdr)) < 0) && (EINTR == errno))
        ;
    if (res < 0) {
        if (ENOMEM == errno)
            return -2;
        perror("writing (SG_IO) on sg device, error");
        return -1;
    }

    if (verbose > 2)
        fprintf(stderr, "      duration=%u ms\n", io_hdr.duration);
    res = sg_err_category3(&io_hdr);
    switch (res) {
    case SG_LIB_CAT_CLEAN:
        break;
    case SG_LIB_CAT_RECOVERED:
        ++recovered_errs;
        info_valid = sg_get_sense_info_fld(io_hdr.sbp, io_hdr.sb_len_wr,
                                           &io_addr);
        if (info_valid) {
            fprintf(stderr, "    lba of last recovered error in this "
                    "WRITE=0x%" PRIx64 "\n", io_addr);
            if (verbose > 1)
                sg_chk_n_print3("writing", &io_hdr, 1);
        } else {
            fprintf(stderr, "Recovered error: [no info] writing to "
                    "block=0x%" PRIx64 ", num=%d\n", to_block, blocks);
            sg_chk_n_print3("writing", &io_hdr, verbose > 1);
        }
        break;
    case SG_LIB_CAT_ABORTED_COMMAND:
    case SG_LIB_CAT_UNIT_ATTENTION:
        sg_chk_n_print3("writing", &io_hdr, verbose > 1);
        return res;
    case SG_LIB_CAT_NOT_READY:
        ++unrecovered_errs;
        fprintf(stderr, "device not ready (w)\n");
        return res;
    case SG_LIB_CAT_MEDIUM_HARD:
    default:
        sg_chk_n_print3("writing", &io_hdr, verbose > 1);
        ++unrecovered_errs;
        if (ofp->coe) {
            fprintf(stderr, ">> ignored errors for out blk=%" PRId64 " for "
                    "%d bytes\n", to_block, bs * blocks);
            return 0; /* fudge success */
        } else
            return res;
    }

    if (diop && *diop &&
        ((io_hdr.info & SG_INFO_DIRECT_IO_MASK) != SG_INFO_DIRECT_IO))
        *diop = 0;      /* flag that dio not done (completely) */
    return 0;
}


int
do_write_same(int sg_fd, const struct opts_t * op, const void * dataoutp,
              int * act_cdb_lenp)
{
    int k, ret, res, sense_cat, cdb_len, cat;
    uint64_t llba;
    uint32_t lba, unum;
    unsigned char wsCmdBlk[WRITE_SAME32_LEN];
    unsigned char sense_b[SENSE_BUFF_LEN];
    struct sg_pt_base * ptvp;

    cdb_len = op->pref_cdb_size;
    if (WRITE_SAME10_LEN == cdb_len) {
        llba = op->lba + op->numblocks;
        if ((op->numblocks > 0xffff) || (llba > ULONG_MAX) ||
            op->ndob || (op->unmap && (0 == op->want_ws10))) {
            cdb_len = WRITE_SAME16_LEN;
            if (op->verbose)
                fprintf(stderr, "do_write_same: use WRITE SAME(16) instead "
                        "of 10 byte cdb\n");
        }
    }
    if (act_cdb_lenp)
        *act_cdb_lenp = cdb_len;
    memset(wsCmdBlk, 0, sizeof(wsCmdBlk));
    switch (cdb_len) {
    case WRITE_SAME10_LEN:
        wsCmdBlk[0] = WRITE_SAME10_OP;
        wsCmdBlk[1] = ((op->wrprotect & 0x7) << 5);
        /* ANCHOR + UNMAP not allowed for WRITE_SAME10 in sbc3r24+r25 but
         * a proposal has been made to allow it. Anticipate approval. */
        if (op->anchor)
            wsCmdBlk[1] |= 0x10;
        if (op->unmap)
            wsCmdBlk[1] |= 0x8;
        if (op->pbdata)
            wsCmdBlk[1] |= 0x4;
        if (op->lbdata)
            wsCmdBlk[1] |= 0x2;
        lba = (uint32_t)op->lba;
        for (k = 3; k >= 0; --k) {
            wsCmdBlk[2 + k] = (lba & 0xff);
            lba >>= 8;
        }
        wsCmdBlk[6] = (op->grpnum & 0x1f);
        wsCmdBlk[7] = ((op->numblocks >> 8) & 0xff);
        wsCmdBlk[8] = (op->numblocks & 0xff);
        break;
    case WRITE_SAME16_LEN:
        wsCmdBlk[0] = WRITE_SAME16_OP;
        wsCmdBlk[1] = ((op->wrprotect & 0x7) << 5);
        if (op->anchor)
            wsCmdBlk[1] |= 0x10;
        if (op->unmap)
            wsCmdBlk[1] |= 0x8;
        if (op->pbdata)
            wsCmdBlk[1] |= 0x4;
        if (op->lbdata)
            wsCmdBlk[1] |= 0x2;
        if (op->ndob)
            wsCmdBlk[1] |= 0x1;
        llba = op->lba;
        for (k = 7; k >= 0; --k) {
            wsCmdBlk[2 + k] = (llba & 0xff);
            llba >>= 8;
        }
        unum = op->numblocks;
        for (k = 3; k >= 0; --k) {
            wsCmdBlk[10 + k] = (unum & 0xff);
            unum >>= 8;
        }
        wsCmdBlk[14] = (op->grpnum & 0x1f);
        break;
    case WRITE_SAME32_LEN:
        /* Note: In Linux at this time the sg driver does not support
         * cdb_s > 16 bytes long, but the bsg driver does. */
        wsCmdBlk[0] = VARIABLE_LEN_OP;
        wsCmdBlk[6] = (op->grpnum & 0x1f);
        wsCmdBlk[7] = WRITE_SAME32_ADD;
        wsCmdBlk[8] = ((WRITE_SAME32_SA >> 8) & 0xff);
        wsCmdBlk[9] = (WRITE_SAME32_SA & 0xff);
        wsCmdBlk[10] = ((op->wrprotect & 0x7) << 5);
        if (op->anchor)
            wsCmdBlk[10] |= 0x10;
        if (op->unmap)
            wsCmdBlk[10] |= 0x8;
        if (op->pbdata)
            wsCmdBlk[10] |= 0x4;
        if (op->lbdata)
            wsCmdBlk[10] |= 0x2;
        if (op->ndob)
            wsCmdBlk[10] |= 0x1;
        llba = op->lba;
        for (k = 7; k >= 0; --k) {
            wsCmdBlk[12 + k] = (llba & 0xff);
            llba >>= 8;
        }
        unum = op->numblocks;
        for (k = 3; k >= 0; --k) {
            wsCmdBlk[28 + k] = (unum & 0xff);
            unum >>= 8;
        }
        break;
    default:
        fprintf(stderr, "do_write_same: bad cdb length %d\n", cdb_len);
        return -1;
    }

    if (op->verbose > 1) {
        fprintf(stderr, "    Write same(%d) cmd: ", cdb_len);
        for (k = 0; k < cdb_len; ++k)
            fprintf(stderr, "%02x ", wsCmdBlk[k]);
        fprintf(stderr, "\n    Data-out buffer length=%d\n",
                op->xfer_len);
    }
    if ((op->verbose > 3) && (op->xfer_len > 0)) {
        fprintf(stderr, "    Data-out buffer contents:\n");
        //dStrHexErr((const char *)dataoutp, op->xfer_len, 1);
    }
    ptvp = construct_scsi_pt_obj();
    if (NULL == ptvp) {
        fprintf(sg_warnings_strm, "Write same(%d): out of memory\n", cdb_len);
        return -1;
    }
    set_scsi_pt_cdb(ptvp, wsCmdBlk, cdb_len);
    set_scsi_pt_sense(ptvp, sense_b, sizeof(sense_b));
    set_scsi_pt_data_out(ptvp, (unsigned char *)dataoutp, op->xfer_len);
    res = do_scsi_pt(ptvp, sg_fd, op->timeout, op->verbose);
    ret = sg_cmds_process_resp(ptvp, "Write same", res, 0, sense_b,
                               1 /*noisy */, op->verbose, &sense_cat);
    if (-1 == ret)
        ;
    else if (-2 == ret) {
        switch (sense_cat) {
        case SG_LIB_CAT_RECOVERED:
        case SG_LIB_CAT_NO_SENSE:
            ret = 0;
            break;
        case SG_LIB_CAT_MEDIUM_HARD:
            {
                int valid, slen;
                uint64_t ull = 0;

                slen = get_scsi_pt_sense_len(ptvp);
                valid = sg_get_sense_info_fld(sense_b, slen, &ull);
                if (valid)
                    fprintf(stderr, "Medium or hardware error starting at "
                            "lba=%" PRIu64 " [0x%" PRIx64 "]\n", ull, ull);
            }
            ret = sense_cat;
            break;
        default:
            ret = sense_cat;
            break;
        }
    } else
        ret = 0;

    destruct_scsi_pt_obj(ptvp);
    return ret;
}

