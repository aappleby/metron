const WASI_ESUCCESS = 0;
const WASI_EBADF = 8;
const WASI_EINVAL = 28;
const WASI_ENOSYS = 52;
const WASI_STDOUT_FILENO = 1;

class WasiStub {

  constructor() {
    this.memory = memory;
  }

  static memory;

  static args_get(argv, argvBuf) {
    console.log("args_get");
    return WASI_ESUCCESS;
  }

  static args_sizes_get(argc, argvBufSize) {
    console.log("args_sizes_get");
    var view = new DataView(WasiStub.memory.buffer);
    view.setUint32(argc, 0, !0);
    view.setUint32(argvBufSize, 0, !0);
    return WASI_ESUCCESS;
  }

  static environ_get(environ, environBuf) {
    console.log("environ_get");
    return WASI_ESUCCESS;
  }

  static environ_sizes_get(environCount, environBufSize) {
    console.log("environ_sizes_get");
    var view = new DataView(WasiStub.memory.buffer);
    view.setUint32(environCount, 0, !0);
    view.setUint32(environBufSize, 0, !0);
    return WASI_ESUCCESS;
  }

  static clock_res_get(clock_id, resolution) {
    /*
    let host_resolution = _malloc(8);
    let ret = ___wasi_clock_res_get(clock_id, host_resolution);
    copyout_i64(resolution, host_resolution);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }

  static clock_time_get(clock_id, precision, time) {
    //console.log("clock_time_get");
    let view = new DataView(WasiStub.memory.buffer);
    view.setBigUint64(time, BigInt(Math.trunc(performance.now() * 1e6)), true);
    return WASI_ESUCCESS;
  }

  static fd_advise(fd, offset, len, advice) {
    //return ___wasi_fd_advise(fd, offset, len, advice);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_allocate(fd, offset, len) {
    //return ___wasi_fd_allocate(fd, offset, len);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_close(fd) {
    //return ___wasi_fd_close(fd);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_datasync(fd) {
    //return ___wasi_fd_datasync(fd);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_fdstat_get(fd, bufPtr) {
    /*
    console.log("fd_fdstat_get");
    let view = new DataView(WasiStub.memory.buffer);
    view.setUint32(bufPtr + 0,  fd, true);
    view.setUint32(bufPtr + 4,  0), true;
    view.setUint32(bufPtr + 8,  0), true;
    view.setUint32(bufPtr + 12, 0), true;
    view.setUint32(bufPtr + 16, 0), true;
    view.setUint32(bufPtr + 20, 0), true;
    return WASI_ESUCCESS;
    */


    var view = new DataView(WasiStub.memory.buffer);

    view.setUint8(bufPtr, fd);
    view.setUint16(bufPtr + 2, 0, !0);
    view.setUint16(bufPtr + 4, 0, !0);

    function setBigUint64(byteOffset, value, littleEndian) {

        var lowWord = value;
        var highWord = 0;

        view.setUint32(littleEndian ? 0 : 4, lowWord, littleEndian);
        view.setUint32(littleEndian ? 4 : 0, highWord, littleEndian);
   }

    setBigUint64(bufPtr + 8, 0, !0);
    setBigUint64(bufPtr + 8 + 8, 0, !0);

    return WASI_ESUCCESS;
  }
  static fd_fdstat_set_flags(fd, flags) {
    //return ___wasi_fd_fdstat_set_flags(fd, flags);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_fdstat_set_rights(fd, fs_rights_base, fs_rights_inheriting) {
    //return ___wasi_fd_fdstat_set_rights(fd, fs_rights_base, fs_rights_inheriting);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_filestat_get(fd, buf) {
    /*
    let host_buf = _malloc(56); // sizeof __wasi_filestat_t
    let ret = ___wasi_fd_filestat_get(host_buf);
    copyout_bytes(buf, host_buf, 56);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static fd_filestat_set_size(fd, size) {
    //return ___wasi_fd_filestat_set_size(fd, size);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_filestat_set_times(fd, st_atim, st_mtim, fstflags) {
    //return ___wasi_fd_filestat_set_times(fd, st_atim, st_mtim, fstflags);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_pread(fd, iovs, iovs_len, offset, nread) {
    /*
    let host_iovs = translate_iovs(iovs, iovs_len);
    let host_nread = _malloc(4);
    let ret = ___wasi_fd_pread(fd, host_iovs, iovs_len, offset, host_nread);
    copyout_i32(nread, host_nread);
    free_iovs(host_iovs, iovs_len);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static fd_prestat_get(fd, buf) {
    /*
    let host_buf = _malloc(8); // sizeof __wasi_prestat_t
    let ret = ___wasi_fd_prestat_get(fd, host_buf);
    copyout_bytes(buf, host_buf, 8);
    return ret;
    */
    //debugger;
    //return WASI_ENOSYS;
    return WASI_EBADF;
  }
  static fd_prestat_dir_name(fd, path, path_len) {
    /*
    let host_buf = _malloc(path_len);
    let ret = ___wasi_fd_prestat_get(fd, host_buf, path_len);
    copyout_bytes(buf, host_buf, path_len);
    return ret;
    */
    //debugger;
    //return WASI_ENOSYS;
    return WASI_EBADF;
  }
  static fd_pwrite(fd, iovs, iovs_len, offset, nwritten) {
    /*
    let host_iovs = translate_ciovs(iovs, iovs_len);
    let host_nwritten = _malloc(4);
    let ret = ___wasi_fd_pwrite(fd, host_iovs, iovs_len, offset, host_nwritten);
    copyout_i32(nwritten, host_nwritten);
    free_ciovs(host_iovs, iovs_len);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static fd_read(fd, iovs, iovs_len, nread) {
    /*
    let host_iovs = translate_iovs(iovs, iovs_len);
    let host_nread = _malloc(4);
    let ret = ___wasi_fd_read(fd, host_iovs, iovs_len, host_nread);
    copyout_i32(nread, host_nread);
    free_iovs(host_iovs, iovs_len);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static fd_readdir(fd, buf, buf_len, cookie, buf_used) {
    /*
    let host_buf = _malloc(buf_len);
    let host_buf_used = _malloc(4);
    let ret = ___wasi_fd_readdir(fd, buf, buf_len, cookie, host_buf_used);
    copyout_i32(buf_used, host_buf_used);
    copyout_bytes(buf, host_buf, buf_len);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static fd_renumber(from, to) {
    //return ___wasi_fd_renumber(from, to);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_seek(fd, offset, whence, newoffset) {
    /*
    let host_newoffset = _malloc(8);
    let ret = ___wasi_fd_seek(fd, offset, whence, host_newoffset);
    copyout_i64(newoffset, host_newoffset);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static fd_sync(fd) {
    //return ___wasi_fd_sync(fd);
    debugger;
    return WASI_ENOSYS;
  }
  static fd_tell(fd, newoffset) {
    /*
    let host_newoffset = _malloc(8);
    let ret = ___wasi_fd_seek(fd, host_newoffset);
    copyout_i64(newoffset, host_newoffset);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }



  static fd_write(fd, iovs, iovsLen, nwritten) {
    console.log(`fd_write ${fd} ${iovs} ${iovsLen} ${nwritten}`);
    let view = new DataView(WasiStub.memory.buffer);

    /*
    console.log(`fd       = ${fd}`);
    console.log(`iovs     = ${iovs}`);
    console.log(`iovsLen  = ${iovsLen}`);
    console.log(`nwritten = ${nwritten}`);
    */

    let total = 0;

    let log = document.getElementById("log");

    for (let i = 0; i < iovsLen; i++) {
      let ptr = view.getUint32(iovs + (8 * i) + 0, true);
      let len = view.getUint32(iovs + (8 * i) + 4, true);
      console.log(`iov ${i} = ${ptr} : ${len}`);

      let s = decode(WasiStub.memory, ptr, len);
      if (len) {
        //log.innerHTML += `@${fd}: ${s}<p>`;
        log.innerHTML += s;
      }
      total += len;
    }

    console.log(`total ${total}`);

    if (iovsLen > 0 && total == 0) debugger;

    view.setUint32(nwritten, total, true);

    //return total ? WASI_ESUCCESS : WASI_EINVAL;
    return WASI_ESUCCESS;
  }

  static path_create_directory(fd, path, path_len) {
    /*
    let host_path = copyin_bytes(path, path_len);
    let ret = ___wasi_path_create_directory(fd, host_path, path_len);
    _free(host_path);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_filestat_get(fd, path, path_len, buf) {
    /*
    let host_path = copyin_bytes(path, path_len);
    let host_buf = _malloc(56); // sizeof __wasi_filestat_t
    let ret = ___wasi_path_filestat_get(fd, host_path, path_len, host_buf);
    copyout_bytes(buf, host_buf, 56);
    _free(host_path);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_filestat_set_times(fd, path, path_len, st_atim, st_mtim, flags) {
    /*
    let host_path = copyin_bytes(path, path_len);
    let ret = ___wasi_path_filestat_set_times(fd, host_path, st_atim, st_mtim, fstflags);
    _free(host_path);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_link(fd0, path0, path_len0, fd1, path1, path_len1) {
    /*
    let host_path0 = copyin_bytes(path0, path_len0);
    let host_path1 = copyin_bytes(path1, path_len1);
    let ret = ___wasi_path_link(fd, host_path0, path_len0, fd1, host_path1, path1_len);
    _free(host_path1);
    _free(host_path0);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_open(dirfd, dirflags, path, path_len, oflags, fs_rights_base, fs_rights_inheriting, fs_flags, fd) {
    /*
    let host_path = copyin_bytes(path, path_len);
    let host_fd = _malloc(4);
    let ret = ___wasi_path_open(dirfd, dirflags, host_path, path_len, oflags, fs_rights_base, fs_rights_inheriting, fs_flags, host_fd);
    copyout_i32(fd, host_fd);
    _free(host_path);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_readlink(fd, path, path_len, buf, buf_len, buf_used) {
    /*
    let host_path = copyin_bytes(path, path_len);
    let host_buf = _malloc(buf_len);
    let host_buf_used = _malloc(4);
    let ret = ___wasi_path_readlink(fd, path, path_len, buf, buf_len, host_buf_used);
    copyout_i32(buf_used, host_buf_used);
    copyout_bytes(buf, host_buf, buf_len);
    _free(host_path);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_remove_directory(fd, path, path_len, flags) {
    /*
    let host_path = copyin_bytes(path, path_len);
    let ret = ___wasi_path_remove_directory(fd, host_path, path_len, flags);
    _free(host_path);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_rename(fd0, path0, path_len0, fd1, path1, path_len1) {
    /*
    let host_path0 = copyin_bytes(path0, path_len0);
    let host_path1 = copyin_bytes(path1, path_len1);
    let ret = ___wasi_path_rename(fd, host_path0, path_len0, fd1, host_path1, path1_len);
    _free(host_path1);
    _free(host_path0);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_symlink(path0, path_len0, fd, path1, path_len1) {
    /*
    let host_path0 = copyin_bytes(path0, path0_len);
    let host_path1 = copyin_bytes(path1, path1_len);
    let ret = ___wasi_path_symlink(host_path0, path_len0, fd, host_path1, path_len1);
    _free(host_path1);
    _free(host_path0);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static path_unlink_file(fd, path, path_len, flags) {
    /*
    let host_path = copyin_bytes(path, path_len);
    let ret = ___wasi_path_unlink_file(fd, host_path, path_len, flags);
    _free(host_path);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static poll_oneoff(in_, out, nsubscriptions, nevents) {
    /*
    let host_in = copyin_bytes(in_, nsubscriptions * 56); // sizeof __wasi_subscription_t
    let host_out = _malloc(nsubscriptions * 32); // sizeof __wasi_event_t
    let host_nevents = _malloc(4);
    let ret = ___wasi_poll_oneoff(host_in, host_out, host_nevents);
    copyout_bytes(out, host_out, nsubscriptions * 32);
    copyout_i32(nevents, host_nevents);
    _free(host_in);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static proc_exit(rval) {
    console.log(`proc_exit = ${rval}`);
    debugger;
    /*
    let message;
    if (rval == 0) {
        message = "success";
    } else {
        message = "error code " + rval;
    }
    throw new WASIExit(rval, message);
    */
    //debugger;
    //return WASI_ENOSYS;
    return WASI_ESUCCESS;
  }
  static proc_raise(sig) {
    /*
    if (sig == 18 || // SIGSTOP
        sig == 19 || // SIGTSTP
        sig == 20 || // SIGTTIN
        sig == 21 || // SIGTTOU
        sig == 22 || // SIGURG
        sig == 16 || // SIGCHLD
        sig == 13)   // SIGPIPE
    {
      return WASI_ENOSYS;
    }

    let message = "raised signal " + sig;
    throw new WASIExit(128 + sig, message);
    */
    debugger;
    return WASI_ENOSYS;
  }
  static sched_yield() {
    //return ___wasi_sched_yield();
    debugger;
    return WASI_ENOSYS;
  }
  static random_get(buf, buf_len) {
    /*
    let host_buf = _malloc(buf_len);
    let ret = ___wasi_random_get(host_buf, buf_len);
    copyout_bytes(buf, host_buf, buf_len);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static sock_recv(sock, ri_data, ri_data_len, ri_flags, ro_datalen, ro_flags) {
    /*
    let host_ri_data = translate_iovs(ri_data, ri_data_len);
    let host_ro_datalen = _malloc(4);
    let ret = ___wasi_sock_recv(sock, host_ri_data, ri_data_len, ri_flags, host_ro_data, ro_flags);
    copyout_i32(ro_datalen, host_ro_datalen);
    free_iovs(host_ri_data, ri_data_len);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static sock_send(sock, si_data, si_data_len, si_flags, so_datalen) {
    /*
    let host_si_data = translate_ciovs(si_data, si_data_len);
    let host_so_datalen = _malloc(4);
    let ret = ___wasi_sock_send(sock, host_si_data, si_data_len, si_flags, host_so_datalen);
    copyout_i32(so_datalen, host_so_datalen);
    free_ciovs(host_si_data, si_data_len);
    return ret;
    */
    debugger;
    return WASI_ENOSYS;
  }
  static sock_shutdown(sock, how) {
    //return ___wasi_sock_shutdown(sock, how);
    debugger;
    return WASI_ENOSYS;
  }
}
