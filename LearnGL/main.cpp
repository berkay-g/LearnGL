#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "window.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "primitives.h"

#include "fonts\roboto_font.h"

void processInput(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void imguiDock();

const int WIDTH = 1280;
const int HEIGHT = 720;

Camera camera;

float currentFrame = static_cast<float>(glfwGetTime());
float lastFrame = currentFrame;
float deltaTime;

bool kbmActive = false;

#ifdef NDEBUG 
#include <Windows.h> 
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else 
int main(void)
#endif
{
	Window window(WIDTH, HEIGHT, "title");
	glfwSetKeyCallback(window.getWindow(), keyCallback);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	io.ConfigViewportsNoAutoMerge = false;
	io.ConfigViewportsNoTaskBarIcon = false;
	ImGui::StyleColorsDark();

	ImFontConfig fontConfig;
	fontConfig.FontDataOwnedByAtlas = false;
	ImFont* robotoFont = io.Fonts->AddFontFromMemoryCompressedTTF((void*)roboto_compressed_data, roboto_compressed_size, 15.0f, &fontConfig);
	io.FontDefault = robotoFont;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Load and compile shaders
	Shader shader(vshader, fshader);

	Object alliance("resources/models/alliance.obj");
	alliance.rotate(0, glm::vec3(0.f, 180.f, 0.f));
	alliance.scale(0, glm::vec3(2.f));
	Texture alliance_tex("resources/images/alliance.png");

	// Main loop...
	while (!window.shouldClose())
	{
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		processInput(window.getWindow());

		window.clear(0.1f, 0.1f, 0.1f, 1.0f);

		// Activate the shader
		shader.use();

		// Set camera view and projection matrices
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		alliance_tex.bind();
		alliance.draw();


		static bool enable_docking = false;
		if (enable_docking)
			imguiDock();

		//ImGui::ShowDemoWindow();

		{
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				ImGui::SetNextWindowSize({276.f, 382.f}, ImGuiCond_FirstUseEver);
			ImGui::Begin("Controls");
			static glm::vec3 rotationAngle = glm::vec3(0.f, 180.f, 0.f);
			static glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.f);
			static float modelScale = 2.f;
			ImGui::SliderFloat3("Position", glm::value_ptr(modelPos), -5.0f, 5.0f);
			ImGui::SliderFloat("Scale", &modelScale, 0.f, 3.f);
			ImGui::SliderFloat("Rotation X", &rotationAngle.x, 0.f, 360.f);
			ImGui::SliderFloat("Rotation Y", &rotationAngle.y, 0.f, 360.f);
			ImGui::SliderFloat("Rotation Z", &rotationAngle.z, 0.f, 360.f);
			ImGui::SeparatorText("");
			alliance.setPosition(0, modelPos);
			alliance.rotate(0, rotationAngle);
			alliance.scale(0, glm::vec3(modelScale));

			if (ImGui::Button("Activate KBM"))
			{
				int iconified = glfwGetWindowAttrib(window.getWindow(), GLFW_ICONIFIED);

				if (!kbmActive && !iconified)
				{
					kbmActive = true;
					glfwFocusWindow(window.getWindow());
				}
				else
					kbmActive = false;
				
			}
			if (kbmActive)
			{
				ImGui::SameLine();
				ImGui::Text("<<PRESS K TO DISABLE>>");
			}

			if (ImGui::Button("Fullscreen"))
			{
				int iconified = glfwGetWindowAttrib(window.getWindow(), GLFW_ICONIFIED);
				if (!iconified)
					window.setFullScreen(glfwGetWindowMonitor(window.getWindow()) == nullptr);
			}
			if (glfwGetWindowMonitor(window.getWindow()) != nullptr)
			{
				ImGui::SameLine();
				ImGui::Text("<<PRESS F1 TO EXIT FULL SCREEN>>");
			}

			ImGui::Checkbox("Docking", &enable_docking);

			ImGui::Text("Camera Position %.3f %.3f %.3f", camera.position.x, camera.position.y, camera.position.z);
			ImGui::Text("Model Position %.3f %.3f %.3f", alliance.getPosition(0).x, alliance.getPosition(0).y, alliance.getPosition(0).z);

			ImGui::End();
		}

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		// Swap the front and back buffers
		window.swapBuffers();
		// Poll for events
		window.pollEvents();
	}

	return 0;
}

void processInput(GLFWwindow* window)
{
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		static bool focus;
		if (kbmActive)
		{
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
			io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double xpos, ypos;
			int width, height;
			glfwGetCursorPos(window, &xpos, &ypos);

			// Reset mouse position for next frame
			glfwGetWindowSize(window, &width, &height);
			glfwSetCursorPos(window, static_cast<double>(height) / 2.0, static_cast<double>(width) / 2.0);

			camera.ProcessMouseMovement(xpos, ypos, static_cast<float>(height), static_cast<float>(width));


			camera.ProccessKeyboard(0, deltaTime);
			// Move forward
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera.ProccessKeyboard(GLFW_KEY_W, deltaTime);
			// Move backward
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera.ProccessKeyboard(GLFW_KEY_S, deltaTime);
			// Strafe right
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera.ProccessKeyboard(GLFW_KEY_D, deltaTime);
			// Strafe left
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera.ProccessKeyboard(GLFW_KEY_A, deltaTime);
			// Fly up
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				camera.ProccessKeyboard(GLFW_KEY_SPACE, deltaTime);
			// Fly down
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				camera.ProccessKeyboard(GLFW_KEY_LEFT_CONTROL, deltaTime);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Handle keyboard input
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (glfwGetWindowMonitor(window) == nullptr)
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		else
			glfwSetWindowMonitor(window, nullptr, (mode->width - WIDTH) / 2, (mode->height - HEIGHT) / 2, WIDTH, HEIGHT, mode->refreshRate);
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		if (!kbmActive)
		{
			kbmActive = true;
			glfwFocusWindow(window);
		}
		else
			kbmActive = false;
	}
}

void imguiDock()
// Docking
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoSplit;
	dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
	dockspace_flags |= ImGuiDockNodeFlags_AutoHideTabBar;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
		window_flags |= ImGuiWindowFlags_NoBackground;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	//ImGuiID dockspace_id = ImGui::GetMainViewport()->ID;
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	static auto first_time = true;
	if (first_time) {
		first_time = false;
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
		auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
		ImGui::DockBuilderDockWindow("Dear ImGui Demo", dock_id_left);
		ImGui::DockBuilderDockWindow("Controls", dock_id_left);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			ImGui::MenuItem("Fullscreen", NULL, false);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }

			ImGui::Separator();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}