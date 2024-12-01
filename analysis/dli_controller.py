import numpy as np
import matplotlib.pyplot as plt

# Constants
DLI_SETPOINT = 12  # mol/(m^2*day)
controller_ontime_interval_h = 12  # hours
TIME_DAY_H = 24  # hours
CYCLE_START_TIME_H = 6
SECONDS_PER_HOUR = 3600
MOL_TO_UMOL = 1e6
SIMULATION_INTERVAL_H = 0.5


class DLIController:
    def __init__(self):
        self.stored_instantenous_ppfd = []
        self.stored_instantenous_ppfd_setpoint = []
        self.stored_controller_ppfd_output = []
        self.stored_dli_error = []

    def get_controller_output(
        self, instantenous_ppfd, average_expected_ppfd, time_step_h
    ):
        # Store instantaneous and setpoint PPFD
        self.stored_instantenous_ppfd.append(instantenous_ppfd)
        self.stored_instantenous_ppfd_setpoint.append(average_expected_ppfd)

        # Calculate DLI error
        dli_error = 0
        for ppfd in self.stored_instantenous_ppfd_setpoint:
            dli_error += ppfd * time_step_h * SECONDS_PER_HOUR / MOL_TO_UMOL

        for ppfd in self.stored_controller_ppfd_output:
            dli_error -= ppfd * time_step_h * SECONDS_PER_HOUR / MOL_TO_UMOL

        for ppfd in self.stored_instantenous_ppfd:
            dli_error -= ppfd * time_step_h * SECONDS_PER_HOUR / MOL_TO_UMOL

        self.stored_dli_error.append(dli_error)

        # Calculate the controller output
        controller_output = dli_error / (time_step_h * SECONDS_PER_HOUR) * MOL_TO_UMOL
        controller_output = np.clip(controller_output, 0, average_expected_ppfd)
        self.stored_controller_ppfd_output.append(controller_output)

        return controller_output

    @staticmethod
    def get_expected_ontime_ppfd(dli_setpoint, controller_ontime_interval_h):
        seconds_per_day = 86400
        expected_ontime_ppfd = (
            dli_setpoint
            * MOL_TO_UMOL
            / (seconds_per_day * controller_ontime_interval_h / TIME_DAY_H)
        )
        return expected_ontime_ppfd


class Simulator:
    def __init__(
        self,
        dli_controller,
        dli_setpoint,
        controller_ontime_interval_h,
        time_day_h,
        cycle_start_time_h,
    ):
        self.controller = dli_controller
        self.dli_setpoint = dli_setpoint
        self.controller_ontime_interval_h = controller_ontime_interval_h
        self.time_day_h = time_day_h
        self.cycle_start_time_h = cycle_start_time_h

    def simulate(self):
        expected_ontime_ppfd = self.controller.get_expected_ontime_ppfd(
            self.dli_setpoint, self.controller_ontime_interval_h
        )
        print("Expected on-time PPFD:", expected_ontime_ppfd)

        time_intervals = np.arange(
            0, self.time_day_h + SIMULATION_INTERVAL_H, SIMULATION_INTERVAL_H
        )

        time_plotted = []
        controller_expected_ppfd_stored = []
        instantenous_ppfd_stored = []
        controller_output_stored = []
        tot_output_stored = []
        dli_accumulator_stored = []

        for time_h in time_intervals:
            instantenous_ppfd = 0
            controller_expected_ppfd = 0
            if (
                self.cycle_start_time_h
                <= time_h
                < (self.cycle_start_time_h + self.controller_ontime_interval_h)
            ):
                instantenous_ppfd = np.random.uniform(0.7, 0.9) * expected_ontime_ppfd
                controller_expected_ppfd = expected_ontime_ppfd

            controller_expected_ppfd_stored.append(controller_expected_ppfd)
            instantenous_ppfd_stored.append(instantenous_ppfd)

            controller_output = self.controller.get_controller_output(
                instantenous_ppfd, controller_expected_ppfd, SIMULATION_INTERVAL_H
            )
            controller_output_stored.append(controller_output)

            tot_output = instantenous_ppfd + controller_output
            tot_output_stored.append(tot_output)
            tot_output_time_compensated = (
                tot_output / 1e6 * SIMULATION_INTERVAL_H * SECONDS_PER_HOUR
            )
            dli_accumulator_stored.append(
                tot_output_time_compensated
                + (dli_accumulator_stored[-1] if time_h > 0 else 0)
            )

            time_plotted.append(time_h)

        self.plot_results(
            time_plotted,
            controller_expected_ppfd_stored,
            instantenous_ppfd_stored,
            controller_output_stored,
            tot_output_stored,
            dli_accumulator_stored,
        )

    @staticmethod
    def plot_results(
        time_plotted,
        controller_expected_ppfd_stored,
        instantenous_ppfd_stored,
        controller_output_stored,
        tot_output_stored,
        dli_accumulator_stored,
    ):
        fig, axs = plt.subplots(2, sharex=True)
        fig.suptitle("PPFD Controller Simulation")

        # Plot PPFD data
        axs[0].plot(
            time_plotted,
            controller_expected_ppfd_stored,
            label="Controller Expected PPFD",
        )
        axs[0].plot(time_plotted, instantenous_ppfd_stored, label="Instantaneous PPFD")
        axs[0].plot(time_plotted, controller_output_stored, label="Controller Output")
        axs[0].plot(time_plotted, tot_output_stored, label="Total Output")
        axs[0].set(ylabel="PPFD (umol/(m^2*s))")
        axs[0].legend()

        # Plot DLI accumulator
        axs[1].plot(time_plotted, dli_accumulator_stored, label="DLI Accumulator")
        axs[1].plot(time_plotted, dli_controller.stored_dli_error, label="DLI Error")
        axs[1].axhline(y=DLI_SETPOINT, color="r", linestyle="-", label="Reference DLI")
        axs[1].set(ylabel="DLI (mol/m^2)")
        axs[1].legend()

        plt.xlabel("Time (h)")
        plt.show()


if __name__ == "__main__":
    dli_controller = DLIController()
    simulator = Simulator(
        dli_controller,
        DLI_SETPOINT,
        controller_ontime_interval_h,
        TIME_DAY_H,
        CYCLE_START_TIME_H,
    )
    simulator.simulate()
