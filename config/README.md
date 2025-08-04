# MDVSP Solver Configuration

This folder contains INI configuration files for the MDVSP solver.

## Configuration Files

### `mdvsp_config.ini` - Default Configuration
The main configuration file with optimal settings for large MDVSP problems.

### `test_config.ini` - Test Configuration  
Faster settings for small test problems and development.

### `benchmark_config.ini` - Benchmark Configuration
Minimal output and time limits for benchmarking runs.

## INI Format

The configuration uses simple INI format with sections and key=value pairs.
Comments start with # and are ignored.

```ini
# Comments are supported
[section_name]
key = value
another_key = another_value
```

## Configuration Sections

### [problem]
- `input_file`: Path to the problem file (.lp or .mps)
- `file_format`: File format ("auto", "lp", "mps")

### [solver]
- `time_limit`: Maximum solving time in seconds
- `mip_gap`: Relative optimality gap (0.01 = 1%)
- `mip_gap_abs`: Absolute optimality gap
- `threads`: Number of CPU threads (0 = auto)
- `presolve`: Presolve level (0=off, 1=conservative, 2=aggressive)
- `cuts`: Cutting planes level (0=off, 1=conservative, 2=aggressive)
- `heuristics`: Fraction of time spent on heuristics (0.0-1.0)
- `log_to_console`: Show Gurobi log output (true/false)
- `log_interval`: Log update interval in seconds

### [output]
- `solution_file`: Path for solution export
- `log_file`: Path for Gurobi log file
- `export_solution`: Whether to export solution details (true/false)
- `detailed_output`: Show detailed progress information (true/false)

### [analysis]
- `print_problem_stats`: Show problem statistics (true/false)
- `print_solution_details`: Show detailed solution information (true/false)
- `benchmark_mode`: Minimal output for benchmarking (true/false)

## Usage Examples

### Using Default Configuration
```bash
./mdvsp_solver
```

### Using Custom Configuration File
```bash
./mdvsp_solver --config test_config.ini
```

### Command Line Overrides
```bash
./mdvsp_solver --problem-file my_problem.lp --time-limit 1800
```

### Benchmark Mode
```bash
./mdvsp_solver --benchmark --time-limit 60
```

### Quick Test
```bash
./mdvsp_solver --problem-file test_small.lp --time-limit 30 --quiet
```
