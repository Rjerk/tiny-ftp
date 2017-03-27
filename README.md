# A simple ftp client using cpp.

## Command:
- ls
- pwd
- get
- cd
- help
- put
- open
- close
- quit
- delete
- system

## TODO:
- mkdir
- rmdir
- rename
- ...
- more and more

## Build
```
$ git clone https://github.com/Rjerk/ftp-client.git
$ cd ftp-client
$ sh build.sh
```

## Run && Login
```
./ftp 127.0.0.1 21
Connected to 127.0.0.1.
220 (vsFTPd 3.0.3)
Name (127.0.0.1:linux): ftp     # Via anonymous login
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
