assume cs:zs
zs segment

mov ax,2000H
mov ds,ax
mov bx,1000H
mov ax,[bx]
inc bx
inc ax
mov [bx],ax
mov ax,4c00H
int 21H
zs ends
end