#include "fs.h"

typedef struct {
  char *name;  //文件名
  size_t size; //文件大小
  off_t disk_offset;//文件在ramdisk中的偏移
  off_t open_offset;//文件被打开之后的读写操作
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
extern size_t events_read(void *buf, size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void *buf, off_t offset, size_t len);
extern void fb_write(const void *buf, off_t offset, size_t len);
void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size=_screen.height*_screen.width*4;
}
size_t fs_filesz(int fd){
  return file_table[fd].size;//文件大小
}

int fs_open(const char *pathname, int flags, int mode){
  //返回文件描述符fd 即下标
  Log("pathName:%s\n",pathname);
  for(int i=0;i<NR_FILES;i++){
    if(strcmp(file_table[i].name,pathname)==0){
      Log("fd:%d\n",i);
      return i;
    }
  }
  printf("Should not reach here\n");
  assert(0);
  return -1;
}
ssize_t fs_read(int fd, void *buf, size_t len){
  ssize_t fs_size= fs_filesz(fd);
  //限定len长度
  len = (file_table[fd].open_offset + len > fs_size)?(fs_size - file_table[fd].open_offset):len;
  switch (fd)
  {
    case FD_STDIN:break;
    case FD_STDOUT:break;
    case FD_STDERR:
      return 0;
      break;
    case FD_EVENTS:
      len=events_read(buf,len);
      break;
    case FD_DISPINFO:
      //状态信息
      dispinfo_read(buf,file_table[fd].open_offset,len);
      file_table[fd].open_offset += len;
      break;
    default:
      ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
  }
  return len;
}
ssize_t fs_write(int fd, const void *buf, size_t len){
  ssize_t fs_size=fs_filesz(fd);
  switch (fd)
  {
    case FD_STDOUT:
    case FD_STDERR:
      for(int i=0;i<len;i++){
        _putc(((char*)buf)[i]);
      }
      break;
    case FD_FB:
    //fb 帧缓冲区
      fb_write(buf, file_table[fd].open_offset, len);
      file_table[fd].open_offset+=len;
      break;
    default:
    //对于标准输出和标准错误流，以及帧缓冲区，通常不会限制写入的长度
    //而对于文件写入，限制
      len = (file_table[fd].open_offset + len > fs_size)?(fs_size - file_table[fd].open_offset):len;
      if(len<=0)return 0;
      ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
      file_table[fd].open_offset+=len;
      break;
  }
  return len;
}
//调整偏移量
off_t fs_lseek(int fd, off_t offset, int whence){
  off_t ret=-1;
  switch (whence)
  {
    case SEEK_SET:
    //对于文件开头设置偏移量
      if(offset>=0 && offset <= file_table[fd].size){
        file_table[fd].open_offset=offset;
        ret=file_table[fd].open_offset;
      }
      break;
    case SEEK_CUR:
    //相对于当前位置设置偏移量
      if((offset+file_table[fd].open_offset)>=0&&(offset+file_table[fd].open_offset)<=file_table[fd].size){
        file_table[fd].open_offset+=offset;
        ret=file_table[fd].open_offset;
      }
      break;
    case SEEK_END:
    //相对于文件末尾设置偏移量
      file_table[fd].open_offset=file_table[fd].size+offset;
      ret=file_table[fd].open_offset;
      break;
    default:
      Log("it's a undefined whence!\n");
      assert(0);
      break;
  }
  return ret;
}
int fs_close(int fd){
  Log("fd:%d is closing\n",fd);
  return 0;//不用处理
}
