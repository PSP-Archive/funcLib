#include <"whatever.h">

typedef struct {
     short vertU, vertV;
     u32   vertColor;
     short vertX, vertY, vertZ;
} vertTsCVs;

void* motionBuffer = NULL;
vertTsCVs* motionVerts = NULL;

#define MOTION_BUFF_WIDTH 64
#define MOTION_BUFF_HEIGHT 32

void motionInit() {
	motionBuffer = vmemAlloc(MOTION_BUFF_WIDTH * MOTION_BUFF_HEIGHT << 2);
	if(motionBuffer != NULL) {
		motionVerts = memAlign(16, (sizeof(vertTsCVs) << 2));
		if(motionVerts != NULL) {
			motionVerts[0].vertX = 0;
			motionVerts[0].vertY = 0;
			motionVerts[0].vertZ = 0;
			motionVerts[0].vertU = 0;
			motionVerts[0].vertV = 0;
			motionVerts[0].vertColor = 0x7FFFFFFF;
			motionVerts[1].vertX = MOTION_BUFF_WIDTH;
			motionVerts[1].vertY = MOTION_BUFF_HEIGHT;
			motionVerts[1].vertZ = 0;
			motionVerts[1].vertU = SCREEN_WIDTH;
			motionVerts[1].vertV = SCREEN_HEIGHT;
			motionVerts[1].vertColor = 0xFFFFFFFF;

			motionVerts[2].vertX = 0;
			motionVerts[2].vertY = 0;
			motionVerts[2].vertZ = 0;
			motionVerts[2].vertU = 0;
			motionVerts[2].vertV = 0;
			motionVerts[2].vertColor = 0x7FFFFFFF;
			motionVerts[3].vertX = SCREEN_WIDTH;
			motionVerts[3].vertY = SCREEN_HEIGHT;
			motionVerts[3].vertZ = 0;
			motionVerts[3].vertU = MOTION_BUFF_WIDTH;
			motionVerts[3].vertV = MOTION_BUFF_HEIGHT;
			motionVerts[3].vertColor = 0xFFFFFFFF;
		} else
			vmemFree(motionBuffer);
	}
}

void motionAccumulate() { // Call this each frame swap to keep updated
	sceGuDisable(GU_DEPTH_TEST);

	sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
	void* tempDrawBuffer = memUncachedPtr(vmemAbsolutePtr(graphicsDrawBuffer));
	sceGuTexImage(0, SCREEN_PITCH, nextPow2(SCREEN_HEIGHT), SCREEN_PITCH, tempDrawBuffer);

	sceGuDrawBufferList(GU_PSM_8888, memCachedPtr(motionBuffer), MOTION_BUFF_WIDTH);
	
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     

	sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, motionVerts);

	sceGuDrawBufferList(GU_PSM_8888, (void*)graphicsDrawBuffer, SCREEN_STRIDE);
	
	sceGuEnable(GU_DEPTH_TEST);
}

void motionRender() { // Call this to apply the actual effect.
	sceGuDisable(GU_DEPTH_TEST);

	sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
	sceGuTexImage(0, MOTION_BUFF_WIDTH, nextPow2(MOTION_BUFF_HEIGHT), nextPow2(MOTION_BUFF_WIDTH), motionBuffer);

	sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0x00FFFFFF, 0x00FFFFFF);

	sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, &motionVerts[2]);

	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);     

	sceGuEnable(GU_DEPTH_TEST);
}
