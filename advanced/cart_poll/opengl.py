import sys
import ctypes

import glfw
import pyrr
import numpy as np
import open3d as o3d
from OpenGL.GL import *
from OpenGL.GLU import *


class Shader(object):
    def __init__(self):
        self.program = glCreateProgram()

    def attach(self, filename, type):
        shader = self.compile(filename, type)
        glAttachShader(self.program, shader)

    def enable(self):
        glUseProgram(self.program)

    def disable(self):
        glUseProgram(0)

    def link(self):
        glLinkProgram(self.program)

        # リンクの成否を判定する
        # Check whether link is successful
        linkState = glGetProgramiv(self.program, GL_LINK_STATUS)
        if linkState == GL_FALSE:
            # エラーメッセージを取得する
            # Get error message
            errMsg = glGetProgramInfoLog(self.program)

            # エラーメッセージを出力する
            # Print error message
            sys.stderr.write("[ ERROR ] %s\n" % errMsg)

            # リンクに失敗したらエラーメッセージを表示して終了
            # Terminate with error message if link is failed
            raise Exception("Failed to link shaders!")

        # シェーダを無効化した後にIDを返す
        # Disable shader program and return its ID
        glUseProgram(0)

    def compile(self, filename, type):
        # シェーダの作成
        # Create a shader
        shaderId = glCreateShader(type)

        # ファイルをすべて読んで変数に格納
        # Load entire contents of a file and store to a string variable
        with open(filename, 'r', encoding='utf-8') as f:
            code = f.read()

        # コードのコンパイル
        # Compile a source code
        glShaderSource(shaderId, code)
        glCompileShader(shaderId)

        # コンパイルの成否を判定する
        # Check whther compile is successful
        compileStatus = glGetShaderiv(shaderId, GL_COMPILE_STATUS)
        if compileStatus == GL_FALSE:
            # エラーメッセージの長さを取得する
            # Get length of error message
            errMsg = glGetShaderInfoLog(shaderId)

            # エラーメッセージとソースコードの出力
            # Print error message and corresponding source code
            sys.stderr.write("[ ERROR ] %s\n" % errMsg)
            sys.stderr.write("%s\n" % code)

            # コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
            # Terminate with error message if compilation failed
            raise Exception("Failed to compile a shader!")

        return shaderId

    def get_uniform_location(self, name):
        return glGetUniformLocation(self.program, name)


class Mesh(object):
    def __init__(self):
        self.n_elems = 0
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)

    def load(self, filename):
        # メッシュファイルの読み込み
        # Load mesh file
        mesh = o3d.io.read_triangle_mesh(filename)
        vertices = np.array(mesh.vertices, dtype='float32')
        if mesh.vertex_normals is not None:
            normals = np.array(mesh.vertex_normals, dtype='float32')
        else:
            normals = np.zeros_like(vertices)
        indices = np.array(mesh.triangles, dtype='uint32')

        # 頂点配列の作成
        # Prepare vertex array
        vertices = np.concatenate([vertices, normals], axis=1)

        # 配列を1次元配列に変換しておく
        # Convert each array to 1D array
        vertices = np.array(vertices, dtype='float32').reshape((-1))
        indices = np.array(indices, dtype='uint32').reshape((-1))

        # 頂点バッファオブジェクトの作成
        # Create vertex buffer object
        vertexBufferId = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId)
        glBufferData(GL_ARRAY_BUFFER, len(vertices.tobytes()), vertices.tobytes(), GL_STATIC_DRAW)

        # 頂点バッファに対する属性情報の設定
        # Setup attributes for vertex buffer object
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * 3 * 2, ctypes.c_void_p(0))

        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * 3 * 2, ctypes.c_void_p(4 * 3))

        # 頂点番号バッファオブジェクトの作成
        # Create index buffer object
        indexBufferId = glGenBuffers(1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, len(indices.tobytes()), indices.tobytes(), GL_STATIC_DRAW)

        # 頂点バッファのサイズを変数に入れておく
        # Store size of index array buffer
        self.n_elems = len(indices)

        # VAOをOFFにしておく
        # Temporarily disable VAO
        glBindVertexArray(0)

    def draw(self):
        glBindVertexArray(self.vao)
        glDrawElements(GL_TRIANGLES, self.n_elems, GL_UNSIGNED_INT, ctypes.c_void_p(0))
        glBindVertexArray(0)
