rebuild: build-mock-jni build-java build-cpp

# Step 1) create P1Bridge.java, get headers/JNI mock

build-mock-jni:
	javac -h . P1Bridge.java

# Step 2) compile Java code

build-java:
	javac P1Bridge.java; javac P1.java;

# Step 3) create P1Bridge.cpp (linked to JNI/jni.h), get dynamic library

build-cpp:
	g++ -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -shared -o libp1bridge.so P1Bridge.cpp

# 3) Run Java code

run:
	@java -Djava.library.path=. P1

clean:
	rm P1.class P1Bridge.class libp1bridge.so
