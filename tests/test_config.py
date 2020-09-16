import pytest

from pypops.simulation import json_to_config


def test_load_json(shared_datadir):
    config = json_to_config(shared_datadir / "simple_config.json")
    assert config.random_seed == 42
    assert config.use_lethal_temperature is True
    assert config.lethal_temperature == pytest.approx(-1.5, abs=0.1)
