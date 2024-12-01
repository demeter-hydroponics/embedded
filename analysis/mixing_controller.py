import numpy as np
import matplotlib.pyplot as plt


class MixingController:
    def __init__(self, lower_pH_limit, upper_pH_limit):
        self.lower_pH_limit = lower_pH_limit
        self.upper_pH_limit = upper_pH_limit

        self.hysteresis_fired = False

    def get_controller_output(self, current_pH_value):
        output = 0

        if current_pH_value > self.upper_pH_limit:
            self.hysteresis_fired = True
            output = 1
        elif current_pH_value < self.lower_pH_limit:
            self.hysteresis_fired = False
            output = 0
        elif self.hysteresis_fired:
            output = 1
        else:
            output = 0

        return output


class MixingSimulator:
    def __init__(self, lower_pH_limit, upper_pH_limit):
        self.lower_pH_limit = lower_pH_limit
        self.upper_pH_limit = upper_pH_limit
        self.current_pH_value = 7.0
        self.num_simulation_ticks = 400

        self.controller_output_stored = []
        self.pH_values_stored = []

        self.controller = MixingController(lower_pH_limit, upper_pH_limit)

    def simulate(self):
        pH_controller_output_gain_per_tick = -0.2
        pH_no_controller_output_gain_per_tick = -pH_controller_output_gain_per_tick / 10

        for _ in range(self.num_simulation_ticks):
            controller_output = self.controller.get_controller_output(
                self.current_pH_value
            )
            if controller_output:
                self.current_pH_value += pH_controller_output_gain_per_tick
            else:
                self.current_pH_value += pH_no_controller_output_gain_per_tick

            self.pH_values_stored.append(self.current_pH_value)

            self.controller_output_stored.append(controller_output)

    def plot_results(self):
        # plot the pH values, as well as the controller output on the same graph
        fig, axs = plt.subplots(2, sharex=True)
        axs[0].plot(self.pH_values_stored, color="b", label="pH values")
        axs[0].set_title("pH values")

        # plot the lower and upper pH limits
        axs[0].axhline(
            self.lower_pH_limit, color="r", linestyle="--", label="Lower pH limit"
        )
        axs[0].axhline(
            self.upper_pH_limit, color="r", linestyle="--", label="Upper pH limit"
        )
        axs[0].legend()

        axs[1].plot(self.controller_output_stored)
        axs[1].set_title("Controller output (Valve open/closed)")
        plt.show()


if __name__ == "__main__":
    simulator = MixingSimulator(5.5, 6.5)
    simulator.simulate()
    simulator.plot_results()
