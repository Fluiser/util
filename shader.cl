#define MAX_ITERATIONS 15

__kernel void SHADERMAIN(
    __global unsigned int* canvas, // 0
    double W_X, // 1
    double W_Y, // 2
    double scale, // 3
    double offsetX, // 4
    double offsetY, // 5
    double c0, // 6
    double c1 // 7
)
{
    int idx = get_global_id(0);
    //z
    
    double x = canvas[idx] >> 16;
    double y = canvas[idx] ^ ((unsigned)x << 16);
    x = (x/W_X - 0.5) / scale - offsetX;
    y = (y/W_Y - 0.5) / scale - offsetY;
    // canvas[idx] = (tx << 16) | ty; // for test
    double tx = x;
    double ty = y;
    canvas[idx] = 0xffffffff;

    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        {//z^2
            double re = x*x - y*y;
            double im = x*y + y*x;

            x = re;
            y = im;
        }
        {// z += c
            x += tx;
            y += ty;
        }
        if(x*x + y*y >= 20)
        {
            unsigned c = 0xff*i/MAX_ITERATIONS; // В пизду, короче.
            // И так заебись.
            canvas[idx] = ((c << 24) | (c << 16) | (c << 8)) | 0x000000ff;
            break;
        }
    }
}
