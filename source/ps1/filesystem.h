#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

int GetFileLength(char *path);
void InitFilesystem(void);
int LoadFile(char *path, void *destination);

#endif // _FILESYSTEM_H_
