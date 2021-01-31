"""
Send exactly the same pulses as written in the csv by finding the length of each pulse and turning on
the ir led for that period of time.
"""
from typing import List, Union, Tuple

import click

from utils import Sample, Signal, convert_samples_to_microseconds, convert_samples_to_milliseconds, dump_signals_to_file_as_cpp_code, format_signals_into_cpp_code, load_file, remove_start_offset

MIN_DIFF_BETWEEN_SIGNALS = 100


def convert_samples_to_signals(samples: List[Sample]) -> List[Signal]:
    signals = []
    for index in range(len(samples) - 1):
        length = samples[index + 1].timestamp - samples[index].timestamp
        value = samples[index].value
        signals.append(Signal(length=length, value=value))
    return signals


def _extract_single_signal(samples: List[Sample]) -> Tuple[Signal, List[Sample]]:
    value = samples[0].value
    start_time = samples[0].timestamp
    for index in range(1, len(samples)):
        diff = samples[index].timestamp - samples[index - 1].timestamp
        if diff > MIN_DIFF_BETWEEN_SIGNALS:
            signal = Signal(value=value, length=samples[index - 1].timestamp - start_time)
            return signal, samples[index - 1:]

    return Signal(value=value, length=samples[-1].timestamp - start_time), []


def join_signals(signals: List[Signal]) -> List[Signal]:
    stack = []
    grouped_signals = []
    for signal in signals:
        if signal.length < MIN_DIFF_BETWEEN_SIGNALS:
            stack.append(signal)
        else:
            if stack:
                new_signal = Signal(value=stack[0].value, length=sum([s.length for s in stack]))
                grouped_signals.append(new_signal)
                stack = []
            grouped_signals.append(signal)

    new_signal = Signal(value=stack[0].value, length=sum([s.length for s in stack]))
    grouped_signals.append(new_signal)
    return grouped_signals


@click.command()
@click.argument('sample_file_path', type=click.Path(exists=True))
def convert_excel_to_raw_data(sample_file_path: str, template_file_path, output_file_path: str):
    samples = load_file(sample_file_path)
    samples_without_offset = remove_start_offset(samples)
    samples_in_micro_seconds = convert_samples_to_microseconds(samples_without_offset)
    signals = convert_samples_to_signals(samples_in_micro_seconds)
    joined_signals = join_signals(signals)
    print(joined_signals)
    print(format_signals_into_cpp_code(joined_signals))


if __name__ == '__main__':
    convert_excel_to_raw_data()
