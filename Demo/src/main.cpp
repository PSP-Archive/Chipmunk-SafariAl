#include <malloc.h>
#include <stdio.h>
#include <vector>
#include <math.h>

#include <chipmunk/chipmunk.h>

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

PSP_MODULE_INFO("Chipmunk Physics Demos", 0, 1, 1);
PSP_HEAP_SIZE_KB(20480);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

// Exit callback
int exit_callback(int arg1, int arg2, void *common) {
          sceKernelExitGame();
          return 0;
}
// Callback thread 
int CallbackThread(SceSize args, void *argp) {
          int cbid;
 
          cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
          sceKernelRegisterExitCallback(cbid);
 
          sceKernelSleepThreadCB();
 
          return 0;
}
// Sets up the callback thread and returns its thread id
int SetupCallbacks(void) {
          int thid = 0;
 
          thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
          if(thid >= 0) {
                    sceKernelStartThread(thid, 0, 0);
          }
 
          return thid;
}

void InitGU(void *fbp0, void *fbp1, unsigned int * list)
{	
	//Init GU
	sceGuInit();
        sceGuStart( GU_DIRECT, list );
	
	// Set Buffers
	sceGuDrawBuffer( GU_PSM_8888, fbp0, BUF_WIDTH );
	sceGuDispBuffer( SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
	sceGuDepthBuffer( (void*)0x110000, BUF_WIDTH);
 
	sceGuOffset( 2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
	sceGuViewport( 2048, 2048, SCR_WIDTH, SCR_HEIGHT);
	sceGuDepthRange( 65535, 0);
	
	// Set Render States
	sceGuScissor( 0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable( GU_SCISSOR_TEST );
	sceGuDepthFunc( GU_GEQUAL );
	
        sceGuShadeModel( GU_SMOOTH );
	sceGuEnable( GU_CLIP_PLANES );
	
	sceGuDisable( GU_DEPTH_TEST );
	sceGuDisable( GU_TEXTURE_2D );
	
        sceGuFinish();
	sceGuSync(0,0);
 
	sceDisplayWaitVblankStart();
	sceGuDisplay(1);
	
	// finish
}

void SetupProjection( void )
{	
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumOrtho(-320, 320, -240, 240, -1, 1);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
}

struct Vertex
{
	float x, y, z;
};

void drawObject(void *ptr, void *unused)
{
	cpShape *shape = (cpShape *)ptr;
	switch(shape->klass->type){
		case CP_POLY_SHAPE:
		{
			sceGuColor(0xFF000000);
			cpBody *body = shape->body;
			cpPolyShape *poly = (cpPolyShape *)shape;
			
			int num = poly->numVerts;
			cpVect *verts = poly->verts;
			
			Vertex * vertices = (Vertex*)sceGuGetMemory(sizeof(Vertex)*(num+1));
			
			cpVect v;
			
			for(int i=0; i<num; i++){
				v = cpvadd(body->p, cpvrotate(verts[i], body->rot));
				vertices[i].x = v.x;
				vertices[i].y = v.y;
				vertices[i].z = 0;
			}
			
			v = cpvadd(body->p, cpvrotate(verts[0], body->rot));
			
			vertices[num].x = v.x;
			vertices[num].y = v.y;
			vertices[num].z = 0;

			sceGumDrawArray(GU_LINE_STRIP, GU_VERTEX_32BITF | GU_TRANSFORM_3D, num+1, 0, vertices);
		}
			break;
			
		case CP_CIRCLE_SHAPE:
		{
			sceGuColor(0xFF000000);
			cpBody *body = shape->body;
			cpCircleShape *circle = (cpCircleShape *)shape;
			cpVect c = cpvadd(body->p, cpvrotate(circle->c, body->rot));
			
			//Circle drawing
			
			const int segs = 15;
			const cpFloat coef = 2.0*M_PI/(cpFloat)segs;
			
			Vertex * vertices = (Vertex*)sceGuGetMemory(sizeof(Vertex)*(segs+2));
			
			for(int n = 0; n <= segs; n++){
				cpFloat rads = n*coef;
				
				vertices[n].x = circle->r*cosf(rads + body->a) + c.x;
				vertices[n].y = circle->r*sinf(rads + body->a) + c.y;
				vertices[n].z = 0;
			}
			
			vertices[(segs+1)].x = c.x;
			vertices[(segs+1)].y = c.y;
			vertices[(segs+1)].z = 0;
			
			sceGumDrawArray(GU_LINE_STRIP, GU_VERTEX_32BITF | GU_TRANSFORM_3D, segs+2, 0, vertices);
		}
			break;
			
		case CP_SEGMENT_SHAPE:
		{
			sceGuColor(0xFF000000);
			cpBody *body = shape->body;
			cpSegmentShape *seg = (cpSegmentShape *)shape;
			cpVect a = cpvadd(body->p, cpvrotate(seg->a, body->rot));
			cpVect b = cpvadd(body->p, cpvrotate(seg->b, body->rot));
			
			Vertex * vertices = (Vertex*)sceGuGetMemory(sizeof(Vertex)*2);
			
			vertices[0].x = a.x;
			vertices[0].y = a.y;
			vertices[0].z = 0;
			
			vertices[1].x = b.x;
			vertices[1].y = b.y;
			vertices[1].z = 0;
			
			sceGumDrawArray(GU_LINE_STRIP, GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2, 0, vertices);
		}
			break;
		default:
			printf("Bad enumeration in drawObject().\n");
	}
}

extern void demo1_init(void);
extern void demo1_update(int);

extern void demo2_init(void);
extern void demo2_update(int);

extern void demo3_init(void);
extern void demo3_update(int);

extern void demo4_init(void);
extern void demo4_update(int);

extern void demo5_init(void);
extern void demo5_update(int);

extern void demo6_init(void);
extern void demo6_update(int);

extern void demo7_init(void);
extern void demo7_update(int);


typedef void (*demo_init_func)(void);
typedef void (*demo_update_func)(int);
typedef void (*demo_destroy_func)(void);

demo_init_func init_funcs[] = {
	demo1_init,
	demo2_init,
	demo3_init,
	demo4_init,
	demo5_init,
	demo6_init,
	demo7_init,
};

demo_update_func update_funcs[] = {
	demo1_update,
	demo2_update,
	demo3_update,
	demo4_update,
	demo5_update,
	demo6_update,
	demo7_update,
};

void demo_destroy(void);

demo_destroy_func destroy_funcs[] = {
	demo_destroy,
	demo_destroy,
	demo_destroy,
	demo_destroy,
	demo_destroy,
	demo_destroy,
	demo_destroy,
};

int demo_index = 0;

int ticks;
cpSpace * space;
cpBody *staticBody;

void demo_destroy(void)
{
	cpSpaceFreeChildren(space);
	cpSpaceFree(space);
	
	cpBodyFree(staticBody);
}

int main(int argc, char **argv)
{	
	static unsigned int __attribute__((aligned(16))) list[262144];
	void *fbp0=0, *fbp1=0;

	pspDebugScreenInit();
	SetupCallbacks();
	InitGU(fbp0, fbp1, list);
	SetupProjection();
	
	cpInitChipmunk();
	
	init_funcs[demo_index]();
	
	int hit;
	
	while(1) {
		sceGuStart( GU_DIRECT, list );
		sceGuClearColor(0xFFFFFFFF);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
		
		SceCtrlData pad;
		sceCtrlReadBufferPositive(&pad, 1);
		
		if(pad.Buttons & PSP_CTRL_CROSS)
		{			
			int old_index = demo_index;
			
			if(!hit)
			{
				demo_index++;
			}
			
			if(demo_index > 6)
			{
				demo_index = 0;
			}
			
			ticks = 0;
	
			if(!hit)
			{
				destroy_funcs[old_index]();
				
				ticks = 0;
				init_funcs[demo_index]();
			}
			hit = 1;
		}
		else
			hit = 0;
		
		cpSpaceHashEach(space->activeShapes, &drawObject, NULL);
		cpSpaceHashEach(space->staticShapes, &drawObject, NULL);
		
		ticks++;
		
		update_funcs[demo_index](ticks);
		
		pspDebugScreenSetOffset((int)fbp0);
		pspDebugScreenSetXY(0,0);

		pspDebugScreenPrintf("%i", demo_index);
		
		sceGuFinish();
		sceGuSync(0,0);
		fbp0 = sceGuSwapBuffers();
	}
	
	sceGuTerm();
	sceKernelExitGame();
	return 0;
}
