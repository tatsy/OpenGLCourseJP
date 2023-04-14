# MacOS Big Sur以降で動かすための設定
# Special settings for working on MacOS Big Sur or later
import platform
import ctypes.util

uname = platform.uname()
if uname.system == "Darwin" and uname.release >= "20.":
    _find_library = ctypes.util.find_library

    def find_library(name):
        if name in ["OpenGL"]:
            return "/System/Library/Frameworks/{0}.framework/{0}".format(name)
        return _find_library(name)

    ctypes.util.find_library = find_library

# 必要なパッケージのインポート
# Import required packages
import argparse
from collections import deque

import numpy as np
from tqdm import tqdm

from env import CartPoll
from agent import Agent, DQNAgent

N_EPISODES = 5000
N_STEPS = 400


def main():
    parser = argparse.ArgumentParser(description="Q-learning with Cart Poll")
    parser.add_argument("--dqn", action="store_true")
    args = parser.parse_args()

    n_succ = 0
    n_fail = 0
    env = CartPoll()
    if args.dqn:
        print("Use DQN")
        player = DQNAgent()
    else:
        print("Use Q-learning")
        player = Agent()

    for e in tqdm(range(N_EPISODES)):
        eps = 0.001 + 0.9 / (1.0 + e)
        env.reset()

        # show = True
        if e % 100 == 0 or e > N_EPISODES - 100:
            show = True
        else:
            show = False

        for i in range(N_STEPS):
            if show:
                env.display(e, i)

            # Choose next action
            s0 = env.state()
            if np.random.random() < eps:
                action = np.random.randint(CartPoll.N_OUTPUT)
            else:
                action = player.play(env.state())

            # Update
            env.update(action)
            s1 = env.state()

            done = (not s1.valid()) or (i == N_STEPS - 1)
            reward = 0.0
            if done:
                if i < N_STEPS - 1:
                    reward = -1.0
                    n_fail += 1
                else:
                    reward = 1.0
                    n_succ += 1

            player.learn(s0, s1, action, reward)

            if not s1.valid():
                break

            if not env.alive():
                return

        # save movie
        if show:
            env.save(f"movie_{e:05d}.mp4")

        if e % (N_EPISODES // 100) == 0:
            ratio = 100.0 * n_succ / (n_succ + n_fail)
            print(f"SUCCESS: {n_succ:d}/{n_succ+n_fail:d} ({ratio:6.2f} %)")
            n_succ = 0
            n_fail = 0


if __name__ == "__main__":
    main()
