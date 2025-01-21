import numpy as np
import matplotlib.pyplot as plt

DLI_SETPOINT = 12  # mol/(m^2*day)
SECONDS_PER_HOUR = 3600
CONTROLLER_ONTIME_S = 12 * SECONDS_PER_HOUR  # seconds
SECONDS_PER_DAY = 24 * SECONDS_PER_HOUR  # seconds
SIMULATION_TIMESTEP_S = 1

MOL_TO_UMOL = 1e6
UMOL_TO_MOL = 1 / MOL_TO_UMOL

# PPFD units; umol/(m^2*s)


class DLIController:
    def __init__(
        self,
        controller_ontime_s_per_day,
        controller_start_time_s,
        dli_setpoint,
        controller_dt=SIMULATION_TIMESTEP_S,
    ):
        self.controller_dt = controller_dt
        self.controller_ontime_s_per_day = controller_ontime_s_per_day
        self.controller_start_time_s = controller_start_time_s
        self.dli_setpoint = dli_setpoint

        self.average_ppfd = (
            dli_setpoint * MOL_TO_UMOL / controller_ontime_s_per_day
        )  # umol/(m^2*s)

        self.max_ppfd = 300  # umol/(m^2*s)

        self.series_gain = self.average_ppfd

        self.K_i = 0.001  # PPFD/DLI_error

        self.ppfd_error_integral = 0  # umol/(m^2*s)
        self.reference_dli = 0  # mol/(m^2)

    def get_reference_DLI(self, time_s):
        return self.reference_dli

    def get_ppfd_reference(self, time_s):
        if (time_s > self.controller_start_time_s) and (
            time_s < self.controller_start_time_s + self.controller_ontime_s_per_day
        ):
            return self.average_ppfd
        else:
            return 0

    def integrate_dli_reference(self, time_s):
        self.reference_dli += (
            self.get_ppfd_reference(time_s) * UMOL_TO_MOL * self.controller_dt
        )

    def get_controller_output(self, measured_ppfd, time_s):
        self.integrate_dli_reference(time_s)
        ppfd_error = self.get_ppfd_reference(time_s) - measured_ppfd
        self.ppfd_error_integral += ppfd_error * self.controller_dt

        commanded_ppfd = (
            self.series_gain * self.ppfd_error_integral * self.K_i
        )  # umol/(m^2*s)

        commanded_ppfd = np.clip(commanded_ppfd, 0, self.max_ppfd)

        return commanded_ppfd


class SunlightModel:
    def __init__(self, time_vectors, ppfd_vectors):
        self.time_vectors = time_vectors
        self.ppfd_vectors = ppfd_vectors

    def get_sunlight_injected_ppfd(self, time_s):
        return np.interp(time_s, self.time_vectors, self.ppfd_vectors)


class Simulator:
    def __init__(self, dli_controller, sunlight_model):
        self.dli_controller = dli_controller
        self.sunlight_model = sunlight_model

        self.measured_ppfd = []
        self.received_dli = []
        self.dli_reference = []
        self.controller_output = []
        self.sunlight_injected_ppfd = []
        self.time_s = []

    def simulate(self):
        for time_s in range(0, SECONDS_PER_DAY, SIMULATION_TIMESTEP_S):
            sunlight_injected_ppfd = self.sunlight_model.get_sunlight_injected_ppfd(
                time_s
            )

            measured_ppfd = 0
            received_dli = 0

            if time_s > 0:
                measured_ppfd = self.controller_output[-1] + sunlight_injected_ppfd
                instaneous_dli_gain = (
                    measured_ppfd * UMOL_TO_MOL * SIMULATION_TIMESTEP_S
                )
                received_dli = self.received_dli[-1] + instaneous_dli_gain

            self.sunlight_injected_ppfd.append(sunlight_injected_ppfd)
            self.measured_ppfd.append(measured_ppfd)
            self.received_dli.append(received_dli)
            self.controller_output.append(
                self.dli_controller.get_controller_output(measured_ppfd, time_s)
            )
            self.dli_reference.append(self.dli_controller.get_reference_DLI(time_s))
            self.time_s.append(time_s)

    def plot(self):
        fig, axs = plt.subplots(2, 1, figsize=(10, 10))
        axs[0].plot(self.time_s, self.dli_reference, label="DLI Reference")
        axs[0].plot(self.time_s, self.received_dli, label="Received DLI")
        axs[0].set_ylabel("DLI (mol/m^2)")
        axs[0].legend()

        axs[1].plot(self.time_s, self.measured_ppfd, label="Measured PPFD")
        axs[1].plot(self.time_s, self.controller_output, label="Controller Output")
        axs[1].plot(
            self.time_s, self.sunlight_injected_ppfd, label="Sunlight Injected PPFD"
        )
        axs[1].set_ylabel("PPFD (umol/(m^2*s))")
        axs[1].legend()

        plt.show()


if __name__ == "__main__":
    sunlight_model_time_vectors = [
        0,
        6 * SECONDS_PER_HOUR,
        12 * SECONDS_PER_HOUR,
        14 * SECONDS_PER_HOUR,
        15 * SECONDS_PER_HOUR,
        18 * SECONDS_PER_HOUR,
        20 * SECONDS_PER_HOUR,
    ]
    sunlight_model_ppfd_vectors = [0, 130, 130, 380, 240, 30, 0]
    sunlight_model = SunlightModel(
        sunlight_model_time_vectors, sunlight_model_ppfd_vectors
    )

    dli_controller = DLIController(
        CONTROLLER_ONTIME_S, 6 * SECONDS_PER_HOUR, DLI_SETPOINT
    )
    simulator = Simulator(dli_controller, sunlight_model)
    simulator.simulate()
    simulator.plot()
