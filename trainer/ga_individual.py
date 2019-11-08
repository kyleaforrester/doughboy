#!/usr/bin/env python3

import random
import chess
import chess.uci
import chess.pgn
import numpy as np
import multiprocessing as mp
import copy
import datetime
import math
import py_nn

class Individual:

    #Convolutional Neural Network
    #Input is 8x8x12 image
    #Convolution part has 3x3x16 filters
    #Convolution part has 4 layers.  The last convolution layer squashes the image to a 8x8x1 image.
    #The 8x8x1 image is then fed as input to a fully-connected layer.
    #4 new input nodes are then added ot the engine for castline rights for 68 total.
    #Fully connected layer has 1 hidden layers of size 68, and 1 sigmoid output neuron
    #All other neurons use RELU activation except the output neuron
    #There are 5 conv layers and 2 fc layers, for 7 total.
    #Each layer gets its own index in the layers and mutation_rate lists
    mutation_rates = [0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05]
    conv_layers = 5
    sample_size = 128
    curr_positions = []

    def __init__(self, weights_layers=None, biases_layers=None, weights=None, biases=None, evaluation = 0):
        self.evaluation = evaluation
        self.conv_layers = Individual.conv_layers

        if weights_layers is None:
            self.weights_layers = [[3, 3, 12, 16], [3, 3, 16, 16], [3, 3, 16, 16], [3, 3, 16, 16], [3, 3, 16, 1]]
            #Fully connected layer has 4 extra nodes for castling rights
            self.weights_layers += [[68, 68], [68, 1]]
        else:
            self.weights_layers = weights_layers

        if biases_layers is None:
            self.biases_layers = [[16], [16], [16], [16], [1]]
            #Fully connected layer has 4 extra nodes for castling rights
            self.biases_layers += [[68], [1]]
        else:
            self.biases_layers = biases_layers

        if weights is None:
            #Initialize weights
            #Conv layers
            self.weights = [[[[[random.gauss(0,0.05) for a in range(dims[0])] for b in range(dims[1])] for c in range(dims[2])] for d in range(dims[3])] for dims in self.weights_layers[:self.conv_layers]]
            #FC layers
            self.weights += [[[random.gauss(0, 0.05) for a in range(dims[0])] for b in range(dims[1])] for dims in self.weights_layers[self.conv_layers:]]
        else:
            self.weights = weights

        if biases is None:
            #Initialize biases
            #Convolutional biases
            #FC biases
            self.biases = [[random.gauss(0, 0.05) for a in range(dim[0])] for dim in self.biases_layers]
        else:
            self.biases = biases

    def copy(self):
        return Individual(weights=copy.deepcopy(self.weights), weights_layers=self.weights_layers, biases_layers=self.biases_layers, biases=copy.deepcopy(self.biases))
        
    def mutate(self, i):
        mutate_rate = Individual.mutation_rates[i]
        if (i < self.conv_layers):
            #Mutate conv_layer weights and bias
            #Weights
            for a in range(len(self.weights[i])):
                for b in range(len(self.weights[i][a])):
                    for c in range(len(self.weights[i][a][b])):
                        for d in range(len(self.weights[i][a][b][c])):
                            self.weights[i][a][b][c][d] += random.gauss(0, mutate_rate)
            #Biases
            for a in range(len(self.biases[i])):
                self.biases[i][a] += random.gauss(0, mutate_rate)
        else:
            #Mutate FC layer weights and bias
            #Weights
            for a in range(len(self.weights[i])):
                for b in range(len(self.weights[i][a])):
                    self.weights[i][a][b] += random.gauss(0, mutate_rate)

            #Biases
            for a in range(len(self.biases[i])):
                self.biases[i][a] += random.gauss(0, mutate_rate)
        return self

    def evaluate(ind):
        py_nn.setup_py_objects(ind.weights_layers[:ind.conv_layers], ind.weights_layers[ind.conv_layers:], ind.biases_layers[:ind.conv_layers], ind.biases_layers[ind.conv_layers:], ind.weights[:ind.conv_layers], ind.weights[ind.conv_layers:], ind.biases[:ind.conv_layers], ind.biases[ind.conv_layers:])
        error = 0
        for pos in Individual.curr_positions:
            ind_eval = py_nn.py_fire(pos[0])
            error += abs(ind_eval - float(pos[1]))**2
        py_nn.close_py_objects()
        return error

    def play(ind_list, layer_idx):
        now = datetime.datetime.now()
        fd_out = open('games/{}_{}_{}_{}_{}_{}.txt'.format(now.year, now.month, now.day, now.hour, now.minute, now.second), 'w')

        positions = open('data/data.txt').readlines()

        Individual.curr_positions = copy.deepcopy(random.sample(positions, Individual.sample_size))
        Individual.curr_positions = [pos.strip().split(',') for pos in Individual.curr_positions]
        Individual.curr_positions = [(pos[0], pos[3]) for pos in Individual.curr_positions]
        del positions

        #with mp.Pool() as pool:
        #    evals = pool.map(Individual.evaluate, ind_list)
        #    for i in range(len(ind_list)):
        #        ind_list[i].evaluation = evals[i]
        #Replace the below code when ready for multiprocessing
        for ind in ind_list:
            ind.evaluation = Individual.evaluate(ind)

        fd_out.write('Iteration {} Mutation standard deviation: {}\n'.format(layer_idx, Individual.mutation_rates))
        for ind in ind_list:
            fd_out.write('Total Error: {}\n'.format(ind.evaluation))
        fd_out.write('Best Error: {}'.format(min(ind_list, key=lambda x: x.evaluation).evaluation))

    def to_string(self):
        return '{}|{}|{}|{}|{}'.format(self.evaluation, self.weights_layers, self.biases_layers, self.weights, self.biases)

    def read(string):
        p_str = string.split('|')
        return Individual(weights_layers=eval(p_str[1]), biases_layers=eval(p_str[2]), weights=eval(p_str[3]), biases=eval(p_str[4]), evaluation=float(p_str[0]))

