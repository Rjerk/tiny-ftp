# A simple ftp using cpp.

## Build

```
$ git clone https://github.com/Rjerk/tiny-ftp.git
$ cd tiny-ftp
$ make
```

## Run && Login

run ftp-server
```
./ftpserver 2100
```

run ftp-client
```
./ftp localhost 2100
Connected to localhost.
220 (rftp 0.1)
Name (localhost:lan): sam
331 Please specify the password.
Password:
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> 
```

## Command Usage

### cd
- change remote working directory.
- Usage:
```
cd [target-directory]
```

### get
- receive file.
- Usage:
```
get [remote-file] [local-file]
```

### ls
- list contents of remote directory.
- Usage:
```
ls
```

### help
- print local help information
- Usage:
```
help
```

### pwd
- print working directory on remote machine
- Usage:
```
pwd
```

### put
- send one file
- Usage:
```
put [local-file] [remote-file]
```

### open
- connect to remote ftp
- Usage:
```
open [hostname]
```

### close
- terminate ftp session
- Usage:
```
close
```

### quit
- terminate ftp session and exit
- Usage:
```
quit
```

### delete
- delete remote file
- Usage:
```
delete [remote-file]
```

### system
- show remote system type
- Usage:
```
system
```
