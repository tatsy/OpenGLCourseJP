# コンパイラの設定
CC          := clang
CXX         := clang++

# デバッガの設定
DBG		 	:= lldb

# その他のコマンドの設定
RM          := rm
SH          := bash

# ソースコードの設定 (ファイルを追加する場合はここに足す)
SRC         := main.cpp
OBJS        := $(patsubst %.cpp, %.o, $(SRC))
OBJS_DBG  	:= $(patsubst %.cpp, %.debug.o, $(SRC))
DEPS        := $(patsubst %.cpp, %.d, $(SRC))
DEPS_DBG    := $(patsubst %.cpp, %.debug.d, $(SRC))

# コンパイラ引数の設定 (インクルード・ディレクトリ等)
CFLAGS      := -Wall -MP -MMD -I/usr/include -I/usr/local/include -I/opt/homebrew/include -I../../support -DGL_SILENCE_DEPRECATION
CXXFLAGS    := -std=c++11 $(CFLAGS)
CFLAGS_DBG  := -g -O0

# フレームワークの設定 (Mac特有のもの)
FRAMEWORKS  := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# リンカ引数の設定
LDFLAGS     := -L/usr/lib -L/usr/local/lib -L/opt/homebrew/lib -lglfw

# 出来上がるバイナリの名前
RELEASE_EXE := main_exe
DEBUG_EXE	:= main_exe.d

# allターゲットの設定
.PHONY: all
all: $(RELEASE_EXE) $(DEBUG_EXE)

# 依存ファイルのインクルード
-include $(DEPS)

# ソースコードのコンパイル
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS_DBG)

%.debug.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS_DBG) -c $< -o $@

# プログラムのリンク
$(RELEASE_EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(FRAMEWORKS)

$(DEBUG_EXE): $(OBJS_DBG)
	$(CXX) -o $@ $^ $(LDFLAGS) $(FRAMEWORKS)

# プログラムの実行
.PHONY: run
run: $(RELEASE_EXE)
	@./$(RELEASE_EXE)

.PHONY: debug
debug: $(DEBUG_EXE)
	$(DBG) ./$(DEBUG_EXE)

# コンパイル結果を削除する
.PHONY: clean
clean:
	@$(RM) $(RELEASE_EXE) $(DEBUG_EXE) $(OBJS) $(OBJS_DBG) $(DEPS)
