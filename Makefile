OUT_FILE=raytracer.out
OPENGL_FLAGS = -lXi -lGLEW -lGLU -lm -lGL -lm -lpthread -ldl -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp
all:
	g++ Source/*.cpp -o $(OUT_FILE) -std=c++11 -Wall $(OPENGL_FLAGS)
clean:
	rm $(OUT_FILE) -f
horse: $(OUT_FILE)
	./$(OUT_FILE) Samples/horse.xml
mug: $(OUT_FILE)
	./$(OUT_FILE) Samples/horse_and_mug.xml

