# A simple ftp client using cpp.

## Complete:
- ls
- pwd
- get
- cd
- help

## Imcomplete:
- put
- open
- close
- quit
- ...
- more and more

## Compile
```
sh build.sh
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

