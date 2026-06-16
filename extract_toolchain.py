import tarfile, sys, os, shutil

with tarfile.open(sys.argv[1], 'r:gz') as tar:
    members = tar.getmembers()
    for member in members:
        if member.isdir():
            tar.extract(member, path=sys.argv[2])
        elif member.isfile() and member.linkname == '':
            tar.extract(member, path=sys.argv[2])
    for member in members:
        if member.linkname != '' and not member.islnk():
            src_path = os.path.join(sys.argv[2], member.linkname)
            dst_path = os.path.join(sys.argv[2], member.name)
            if os.path.exists(src_path):
                with open(src_path, 'rb') as fsrc:
                    with open(dst_path, 'wb') as fdst:
                        fdst.write(fsrc.read())
                shutil.copymode(src_path, dst_path)
