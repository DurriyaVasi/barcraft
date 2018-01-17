#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>


using namespace glm;
using namespace std;

static const size_t DIM = 16;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ), gridInfo(DIM), currX(0.0f), currY(0.0f), shiftKeyPressed(false)
{		
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();
	
	initCube();
	
	initSquare();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}	

void A1::initCube()
{
	float vertices[] = {
    		0.0f, 0.0f, 0.0f, 
     		0.0f, 1.0f, 0.0f,  
    		1.0f, 0.0f, 0.0f,  
    		1.0f, 1.0f, 0.0f,   
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,  
                1.0f, 0.0f, 1.0f,  
                1.0f, 1.0f, 1.0f 
	};
	unsigned int indices[] = {  // note that we start from 0!
    		0, 1, 3,   // bottom
    		0, 2, 3,    // bottom
		2, 6, 3,  // side front
		3, 7, 6,
		1, 5, 3,  // side right
		3, 5, 7,
		0, 2, 6,   // side left
		0, 4, 6,  
		0, 1, 5,   // side back
		0, 4, 5,  
		4, 6, 7, // top
		4, 5, 7
	};

	// Create the vertex array to record buffer assignments.
        glGenVertexArrays( 1, &m_cube_vao );
        glBindVertexArray( m_cube_vao );

        // Create the cube vertex buffer
        glGenBuffers( 1, &m_cube_vbo );
        glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof(vertices),
                vertices, GL_STATIC_DRAW );

	glGenBuffers( 1, &m_cube_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_cube_ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
                indices, GL_STATIC_DRAW );

        // Specify the means of extracting the position values properly.
        GLint posAttrib = m_shader.getAttribLocation( "position" );
        glEnableVertexAttribArray( posAttrib );
        glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

        // Reset state to prevent rogue code from messing with *my*
        // stuff!
        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        CHECK_GL_ERRORS;
}; 

void A1::initSquare()
{
	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 3,
		0, 2, 3
	};

	glGenVertexArrays( 1, &m_square_vao);
	glBindVertexArray(m_square_vao);
	
	glGenBuffers(1, &m_square_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_square_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
	
	glGenBuffers(1, &m_square_ebo);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_square_ebo);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	GLint posAttrib = m_shader.getAttribLocation("position");
	
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	
	glBindVertexArray(0);
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        CHECK_GL_ERRORS;
}; 


void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		for (int i = 0; i < 8; i++) {
			ImGui::PushID( i );
                	ImGui::ColorEdit3( "##Colour", colours[i] );
                	ImGui::SameLine();
                	if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
                        	if (gridInfo.getHeight(currX, currY) > 0) {
                                	gridInfo.setColour(currX, currY, current_col);
                        	}
                	}
                	ImGui::PopID();
		}

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

void A1::drawCubes(mat4 W) {
	for (int i = 0; i < DIM; i++) {
		for (int j = 0; j < DIM; j++) {
			int height = gridInfo.getHeight(i, j);
			if (height == 0) {
				continue;
			}
			for (int k = 0; k < height; k++) {
				mat4 T = glm::translate(W, vec3(i, k, j));
				glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( T ) );
				int colourIndex = gridInfo.getColour(i, j);
				glUniform3f( col_uni, colours[colourIndex][0], colours[colourIndex][1], colours[colourIndex][2] );
                		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
		}
	}			
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		glBindVertexArray( m_cube_vao );
		drawCubes(W);

		// Highlight the active square.
		glDisable( GL_DEPTH_TEST );

		glBindVertexArray(m_square_vao);
		W = glm::translate( W, vec3(currX, gridInfo.getHeight(currX, currY), currY));
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );
		glUniform3f( col_uni, 0, 0, 0 );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		 

	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	if (action == GLFW_RELEASE && (key == GLFW_KEY_RIGHT_SHIFT || key == GLFW_KEY_LEFT_SHIFT)) {
		shiftKeyPressed = false;
		eventHandled = true;
	}

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_RIGHT_SHIFT || key == GLFW_KEY_LEFT_SHIFT) {
			shiftKeyPressed = true;	
			eventHandled = true;
		}
		if (key == GLFW_KEY_UP) {
			if (currY == 0) {
				eventHandled = true;
			}	
			else {
				if (shiftKeyPressed) {
					gridInfo.setHeight(currX, currY - (1.0), gridInfo.getHeight(currX, currY));
					gridInfo.setColour(currX, currY - (1.0), gridInfo.getColour(currX, currY));
				}
				currY = currY - (1.0);
				eventHandled = true;
			}
		}
		if (key == GLFW_KEY_DOWN) {
			if (currY == (DIM - 1)) {
				eventHandled = true;
			}
			else {
				if (shiftKeyPressed) {
                                        gridInfo.setHeight(currX, currY + (1.0), gridInfo.getHeight(currX, currY));
                                        gridInfo.setColour(currX, currY + (1.0), gridInfo.getColour(currX, currY));
                                }
				currY = currY + (1.0);
				eventHandled = true;
			}
		}
		if (key == GLFW_KEY_LEFT) {
			if (currX == 0) {
				eventHandled = true;
			}
			else {
				if (shiftKeyPressed) {
                                        gridInfo.setHeight(currX - (1.0), currY, gridInfo.getHeight(currX, currY));
                                        gridInfo.setColour(currX - (1.0), currY, gridInfo.getColour(currX, currY));
                                }
                        	currX = currX - (1.0);
                        	eventHandled = true;
			}
                }
		if (key == GLFW_KEY_RIGHT) {
			if (currX == (DIM - 1)) {
				eventHandled = true;
			}
			else {
				if (shiftKeyPressed) {
                                        gridInfo.setHeight(currX + (1.0), currY, gridInfo.getHeight(currX, currY));
                                        gridInfo.setColour(currX + (1.0), currY, gridInfo.getColour(currX, currY));
                                }	
                        	currX = currX + (1.0);
                        	eventHandled = true;
			}
                }
		if (key == GLFW_KEY_SPACE) {
			int oldHeight = gridInfo.getHeight(currX, currY);
			if (oldHeight == 0) {
				gridInfo.setColour(currX, currY, current_col);
			}
                        gridInfo.setHeight(currX, currY, oldHeight + 1);
                        eventHandled = true;
                }
		if (key == GLFW_KEY_BACKSPACE) {
			int oldHeight = gridInfo.getHeight(currX, currY);
			if (oldHeight > 0) {
                        	gridInfo.setHeight(currX, currY, oldHeight - 1);
			}
                        eventHandled = true;
                }
	}
	return eventHandled;
}
