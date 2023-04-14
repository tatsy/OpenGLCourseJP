import os

import cv2
import glfw
import pyrr
import numpy as np
from OpenGL.GL import *
from OpenGL.GLU import *

from opengl import Mesh, Shader

WIN_WIDTH = 1000
WIN_HEIGHT = 1000
WIN_TITLE = "Cart Pole"

BOX_W = 100
BOX_H = 25

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (128, 128, 128)
RED = (255, 0, 0)

VERT_SHADER_FILE = os.path.join(os.path.dirname(__file__), "shaders", "render.vert")
FRAG_SHADER_FILE = os.path.join(os.path.dirname(__file__), "shaders", "render.frag")

BOX_MESH_FILE = os.path.join(os.path.dirname(__file__), "data", "box.obj")
POLL_MESH_FILE = os.path.join(os.path.dirname(__file__), "data", "poll.obj")
SPH_MESH_FILE = os.path.join(os.path.dirname(__file__), "data", "sphere.obj")


class State(object):
    X_RANGE = (50, 450)
    A_RANGE = (-0.15 * np.pi, 0.15 * np.pi)
    VX_RANGE = (-50, 50)
    VA_RANGE = (-0.15 * np.pi, 0.15 * np.pi)

    def __init__(self, x: float, a: float, vx: float, va: float):
        self.x = x
        self.a = a
        self.vx = vx
        self.va = va

    def to_vec(self):
        x0 = (self.x - State.X_RANGE[0]) / (State.X_RANGE[1] - State.X_RANGE[0])
        a0 = (self.a - State.A_RANGE[0]) / (State.A_RANGE[1] - State.A_RANGE[0])
        vx0 = (self.vx - State.VX_RANGE[0]) / (State.VX_RANGE[1] - State.VX_RANGE[0])
        va0 = (self.va - State.VA_RANGE[0]) / (State.VA_RANGE[1] - State.VA_RANGE[0])
        return np.array([x0, a0, vx0, va0], dtype="float32") * 2.0 - 1.0

    def valid(self) -> bool:
        if self.x < State.X_RANGE[0] or State.X_RANGE[1] < self.x:
            return False
        if self.a < State.A_RANGE[0] or State.A_RANGE[1] < self.a:
            return False
        if self.vx < State.VX_RANGE[0] or State.VX_RANGE[1] < self.vx:
            return False
        if self.va < State.VA_RANGE[0] or State.VA_RANGE[1] < self.va:
            return False

        return True


class BaseEnv(object):
    def __init__(self):
        if glfw.init() == glfw.FALSE:
            raise Exception("Initialization failed!!")

        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, glfw.TRUE)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

        self.window = glfw.create_window(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, None, None)
        if self.window is None:
            glfw.terminate()
            raise Exception("Window creation failed!")

        glfw.make_context_current(self.window)
        print(glGetString(GL_VERSION).decode('ascii'))

        # initialize
        glEnable(GL_DEPTH_TEST)
        glClearColor(0.0, 0.0, 0.0, 1.0)

    def __del__(self):
        glfw.destroy_window(self.window)
        glfw.terminate()


class CartPoll(BaseEnv):
    N_INPUT = 4  # number of parameters
    N_OUTPUT = 2  # number of output states

    def __init__(self):
        super(CartPoll, self).__init__()

        # simulation parameters
        self.x0 = self.x = 250.0
        self.a0 = self.a = 0.0
        self.vx0 = self.vx = 0.0
        self.va0 = self.va = 0.0
        self.F = 20.0

        self.y = 350.0
        self.l = 100.0
        self.g = 9.8
        self.dt = 0.1
        self.m_ball = 5.0
        self.m_cart = 1.0

        self.frames = []
        self._alive = True

        # OpenGL parameters
        self.shader = Shader()
        self.shader.attach(VERT_SHADER_FILE, GL_VERTEX_SHADER)
        self.shader.attach(FRAG_SHADER_FILE, GL_FRAGMENT_SHADER)
        self.shader.link()

        self.box = Mesh()
        self.box.load(BOX_MESH_FILE)
        self.poll = Mesh()
        self.poll.load(POLL_MESH_FILE)
        self.sphere = Mesh()
        self.sphere.load(SPH_MESH_FILE)

    def __del__(self):
        cv2.destroyAllWindows()

    def reset(self):
        self.x = self.x0 + np.random.uniform(State.X_RANGE[0] - self.x0, State.X_RANGE[1] - self.x0) * 0.1
        self.a = np.random.uniform(-np.pi, np.pi) * 0.1
        self.vx = self.vx0
        self.va = self.va0
        self.frames.clear()

    def update(self, action):
        """
        action: 0 (move left) or 1 (move right)
        """

        # Update (Runge-Kutta)
        F = self.F * (action * 2 - 1)
        y = np.array([self.x, self.a, self.vx, self.va])
        k1 = self.dfdt(y, F)
        k2 = self.dfdt(y + 0.5 * k1 * self.dt, F)
        k3 = self.dfdt(y + 0.5 * k2 * self.dt, F)
        k4 = self.dfdt(y + k3 * self.dt, F)
        new_y = y + (k1 + 2.0 * k2 + 2.0 * k3 + k4) * self.dt / 6.0

        self.x, self.a, self.vx, self.va = new_y

    def dfdt(self, params, F):
        x, a, vx, va = params
        sa = np.sin(a)
        ca = np.cos(a)
        a_00 = self.m_cart + self.m_ball
        a_01 = -self.m_ball * self.l * ca
        b_0 = F - self.m_ball * self.l * va * va * sa
        a_10 = ca
        a_11 = -self.l
        b_1 = -self.g * sa

        A = np.array([[a_00, a_01], [a_10, a_11]])
        b = np.array([b_0, b_1])
        v = np.linalg.solve(A, b)

        return np.array([vx, va, v[0], v[1]])

    def state(self) -> State:
        return State(self.x, self.a, self.vx, self.va)

    def alive(self):
        return self._alive

    def display(self, episode, step):
        # title
        glfw.set_window_title(self.window, f"Episode: {episode:d}, Step: {step:d}")

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        cx, cy = 250.0, -250.0
        lightPos = [cx + 50, cy + 50.0, 200.0]
        projMat = pyrr.matrix44.create_perspective_projection(45.0, WIN_WIDTH / WIN_HEIGHT, 0.1, 1000.0)
        viewMat = pyrr.matrix44.create_look_at(
            [cx, cy + 50.0, 500.0],  # 視点の位置
            [cx, cy, 0.0],  # 見ている先
            [0.0, 1.0, 0.0])  # 視界の上方向

        self.shader.enable()

        loc = self.shader.get_uniform_location("u_viewMat")
        glUniformMatrix4fv(loc, 1, GL_FALSE, viewMat)
        loc = self.shader.get_uniform_location("u_projMat")
        glUniformMatrix4fv(loc, 1, GL_FALSE, projMat)
        loc = self.shader.get_uniform_location("u_lightPos")
        glUniform3fv(loc, 1, lightPos)

        # cart
        loc = self.shader.get_uniform_location("u_diffColor")
        glUniform3f(loc, 0.8, 0.0, 0.0)
        loc = self.shader.get_uniform_location("u_modelMat")
        modelMat = pyrr.matrix44.create_identity()
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_scale([100.0, 10.0, 50.0]))
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_translation([self.x, -self.y - 5.0, 0.0]))
        glUniformMatrix4fv(loc, 1, GL_FALSE, modelMat)
        self.box.draw()

        # joint
        loc = self.shader.get_uniform_location("u_diffColor")
        glUniform3f(loc, 0.2, 0.2, 0.2)
        loc = self.shader.get_uniform_location("u_modelMat")
        modelMat = pyrr.matrix44.create_identity()
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_scale([5.0, 5.0, 5.0]))
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_translation([self.x, -self.y, 0.0]))
        glUniformMatrix4fv(loc, 1, GL_FALSE, modelMat)
        self.sphere.draw()

        # poll
        loc = self.shader.get_uniform_location("u_diffColor")
        glUniform3f(loc, 0.8, 0.8, 0.8)
        loc = self.shader.get_uniform_location("u_modelMat")
        modelMat = pyrr.matrix44.create_identity()
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_translation([0.0, 0.5, 0.0]))
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_scale([2.0, self.l, 2.0]))
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_axis_rotation([0.0, 0.0, 1.0], self.a))
        modelMat = np.dot(modelMat, pyrr.matrix44.create_from_translation([self.x, -self.y, 0.0]))
        glUniformMatrix4fv(loc, 1, GL_FALSE, modelMat)
        self.poll.draw()

        self.shader.disable()

        glfw.swap_buffers(self.window)
        glfw.poll_events()

        # save
        byte = glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE)
        byte = np.frombuffer(byte, dtype='uint8').reshape((WIN_HEIGHT, WIN_WIDTH, 4))[:, :, :3]
        byte = np.flipud(byte)[:, :, ::-1]
        self.frames.append(byte)

    def save(self, filename):
        fourcc = cv2.VideoWriter_fourcc(*"mp4v")
        writer = cv2.VideoWriter(filename, fourcc, 24.0, (WIN_WIDTH, WIN_HEIGHT))
        for frame in self.frames:
            writer.write(frame)
