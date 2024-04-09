"""
Copyright (C) 2015-2024, Wazuh Inc.
Created by Wazuh, Inc. <info@wazuh.com>.
This program is free software; you can redistribute it and/or modify it under the terms of GPLv2
"""
from pathlib import Path
from wazuh_testing.tools.monitors import file_monitor
from wazuh_testing.utils import callbacks
from wazuh_testing.constants.paths.logs import WAZUH_LOG_PATH

# Constants & base paths
TEST_DATA_PATH = Path(Path(__file__).parent, 'data')
TEST_CASES_PATH = Path(TEST_DATA_PATH, 'test_cases')
CONFIGURATIONS_PATH = Path(TEST_DATA_PATH, 'configuration_templates')
