from collections import deque

import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F

from env import State, CartPoll

ETA = 0.5
GAMMA = 0.99


class Agent(object):
    N_DIGIT = 10  # number of digitization
    N_STATE = N_DIGIT**CartPoll.N_INPUT  # number of state

    def __init__(self):
        self.Q = np.random.uniform(0.0, 1.0, size=(Agent.N_STATE, CartPoll.N_OUTPUT))

    @staticmethod
    def _digitize(x, x_range) -> int:
        return np.digitize(x, bins=np.linspace(x_range[0], x_range[1], Agent.N_DIGIT + 1)[1:-1])

    @staticmethod
    def _digit(state) -> int:
        dig = np.zeros(CartPoll.N_INPUT, dtype="int32")
        dig[0] = Agent._digitize(state.x, State.X_RANGE)
        dig[1] = Agent._digitize(state.a, State.A_RANGE)
        dig[2] = Agent._digitize(state.vx, State.VX_RANGE)
        dig[3] = Agent._digitize(state.va, State.VA_RANGE)
        return sum([d * Agent.N_DIGIT**i for i, d in enumerate(dig)])

    def play(self, state) -> int:
        d = Agent._digit(state)
        return np.argmax(self.Q[d, :])

    def learn(self, s0, s1, a0, reward):
        d0 = Agent._digit(s0)
        d1 = Agent._digit(s1)
        best = np.max(self.Q[d1, :])
        self.Q[d0, a0] = (1.0 - ETA) * self.Q[d0, a0] + ETA * (reward + GAMMA * best)


class QNetwork(nn.Sequential):
    def __init__(self, n_input, n_output):
        super(QNetwork, self).__init__(
            nn.Linear(n_input, 32),
            # nn.BatchNorm1d(16),
            nn.ReLU(inplace=True),
            nn.Linear(32, 32),
            # nn.BatchNorm1d(16),
            nn.ReLU(inplace=True),
            nn.Linear(32, n_output),
        )


class DQNAgent(object):
    def __init__(self):
        self.q_net = QNetwork(CartPoll.N_INPUT, CartPoll.N_OUTPUT)
        self.q_target = QNetwork(CartPoll.N_INPUT, CartPoll.N_OUTPUT)
        self.device = torch.device("cpu")
        if torch.cuda.is_available():
            self.device = torch.device("cuda")

        self.q_net.to(self.device)
        self.q_target.to(self.device)

        self.steps = 0
        self.optim = torch.optim.RMSprop(self.q_net.parameters(), lr=5.0e-5)
        self.memory = deque([], maxlen=50000)
        self.batchsize = 16
        self.criterion = nn.HuberLoss()

    def play(self, state) -> int:
        self.q_net.eval()
        with torch.no_grad():
            input = torch.from_numpy(state.to_vec())
            input = input.unsqueeze(0).requires_grad_(False)
            input = input.to(self.device)
            output = self.q_net(input)
        return torch.argmax(output, dim=1)[0].item()

    def learn(self, s0, s1, a0, reward):
        self.steps += 1
        if self.steps % 2000 == 0:
            self.q_target.load_state_dict(self.q_net.state_dict())

        self.q_net.train()
        self.memory.append((s0, s1, a0, reward))
        if len(self.memory) > self.batchsize:
            idx = np.random.choice(np.arange(len(self.memory)), self.batchsize, replace=False)
            batch = [self.memory[i] for i in idx]
            in0 = np.stack([mem[0].to_vec() for mem in batch], axis=0)
            in1 = np.stack([mem[1].to_vec() for mem in batch], axis=0)
            act = np.array([mem[2] for mem in batch], dtype="int64")
            rew = np.array([mem[3] for mem in batch], dtype="float32")

            in0 = torch.from_numpy(in0)
            in1 = torch.from_numpy(in1).requires_grad_(False)
            act = torch.from_numpy(act)
            rew = torch.from_numpy(rew)

            in0 = in0.to(self.device)
            in1 = in1.to(self.device)
            act = act.to(self.device)
            rew = rew.to(self.device)

            out0 = self.q_net(in0)
            out1 = self.q_target(in1)
            q0 = torch.gather(out0, 1, act.unsqueeze(1)).squeeze(1)
            q1 = torch.max(out1, dim=1)[0]
            loss = F.smooth_l1_loss(q0, rew + GAMMA * q1)

            self.optim.zero_grad()
            loss.backward()
            self.optim.step()
