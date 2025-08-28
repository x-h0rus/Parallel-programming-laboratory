#include <jni.h>
#include "P1Bridge.h"

void todo () {
  printf("TODO\n");
}

JNIEXPORT void JNICALL Java_P1Bridge_compute(JNIEnv *env, jobject thisObj) {
	todo();
	return;
}
