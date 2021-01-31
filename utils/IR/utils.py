from collections import namedtuple
from typing import List

TEMPLATE_FILE_DATA_PLACE_HOLDER = 'DATA_LINE_PLACE_HOLDER'
Sample = namedtuple('Sample', ['timestamp', 'value'])
Signal = namedtuple('Signal', ['length', 'value'])
KHZ_MODIFIER = 1000


def load_file(path: str, row_divider: str = ',') -> List[Sample]:
    parsed_samples = []
    with open(path) as sample_file:
        for line in sample_file.readlines():
            timestamp, value = line.split(row_divider)
            sample = Sample(timestamp=float(timestamp.strip()), value=int(value.strip()))
            parsed_samples.append(sample)
    return parsed_samples


def remove_start_offset(samples: List[Sample]) -> List[Sample]:
    sampling_start_offset = samples[0].timestamp
    return [Sample(timestamp=sample.timestamp - sampling_start_offset, value=sample.value) for sample in samples]


def frequency_to_milliseconds(frequency: int) -> float:
    return 1.0 / frequency


def convert_samples_to_milliseconds(samples: List[Sample]) -> List[Sample]:
    return [Sample(timestamp=sample.timestamp * 1000, value=sample.value) for sample in samples]


def convert_samples_to_microseconds(samples: List[Sample]) -> List[Sample]:
    return [Sample(timestamp=sample.timestamp * 1000 * 1000, value=sample.value) for sample in samples]


def format_signals_into_cpp_code(signals: List[Signal]) -> str:
    signals_string_format = ", ".join(list(map(lambda signal: str(round(signal.length)), signals)))

    return f"uint16_t data[{len(signals)}] = {{{signals_string_format}}};"


def dump_signals_to_file_as_cpp_code(signals: List[Signal], template_file_path: str, output_file_path: str):
    with open(template_file_path) as template_file:
        template = template_file.read()

    with open(output_file_path, 'w') as output_file:
        output_file.write(template.replace(TEMPLATE_FILE_DATA_PLACE_HOLDER, format_signals_into_cpp_code(signals)))
