#define VIDEO_ADDRESS 0xb8000


extern "C" void main(){
    *(char*)VIDEO_ADDRESS = 'Q';
    return;
}