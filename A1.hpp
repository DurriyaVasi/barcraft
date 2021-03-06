#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:

	void initGrid();

	void initCube();
	void drawCubes(glm::mat4 W, glm::mat4 S);
	void initSquare();

	void reset();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	GLuint m_cube_vao; // Vertex Array Object
        GLuint m_cube_vbo; // Vertex Buffer Object
	GLuint m_cube_ebo; // Element Buffer Object

	GLuint m_square_vao; // Vertex Array Object
        GLuint m_square_vbo; // Vertex Buffer Object
        GLuint m_square_ebo; // Element Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	int current_col;
	Grid gridInfo;
	float currX;
	float currY;
	float colours[8][3] = {
                { 1.0f, 1.0f, 1.0f },
                { 1.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f },
                { 1.0f, 1.0f, 0.0f },
                { 1.0f, 0.0f, 1.0f },
                { 0.0f, 1.0f, 1.0f },
                { 0.5f, 0.5f, 1.0f }
        };
	bool shiftKeyPressed;
	double oldX;
	glm::mat4 oldRotate;
	double oldY;
	bool mousePressed;	
};
