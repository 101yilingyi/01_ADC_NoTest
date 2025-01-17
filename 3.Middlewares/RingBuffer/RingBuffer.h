#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

typedef struct RingBuffInfo{
    unsigned char *pHead;
    unsigned char *pEnd;    
    unsigned char *pValid;    
    unsigned char *pValidEnd; 
    unsigned int  nBufferLength;
    unsigned int  nValidLength;   
}RingBuffInfo;

typedef struct RingBuffer{
    RingBuffInfo info;
    int         (*Write)(struct RingBuffer *ptbuf, const unsigned char *src, unsigned int length);
    int         (*Read)(struct RingBuffer *ptbuf, unsigned char *dst, unsigned int length);
    int         (*Clear)(struct RingBuffer *ptbuf);
    int         (*Free)(struct RingBuffer *ptbuf);
    struct RingBuffer *next;
}RingBuffer;

struct RingBuffer *RingBufferNew(unsigned int length);

#endif /* __DRV_BUFFER_H */

