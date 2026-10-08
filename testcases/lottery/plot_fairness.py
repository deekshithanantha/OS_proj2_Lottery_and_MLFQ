#!/usr/bin/env python3
"""Plot measured Xinu turnaround fairness from a captured console log."""
import argparse
import csv
import re
from collections import defaultdict
from pathlib import Path
from statistics import mean


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('log', type=Path)
    parser.add_argument('--output', type=Path, default=Path('fairness.png'))
    args = parser.parse_args()
    rows = []
    groups = defaultdict(list)
    for match in re.finditer(r'FAIRNESS_DATA,(\d+),(\d+),(\d+),(\d+)',
                             args.log.read_text(errors='replace')):
        runtime, trial, ta, tb = map(int, match.groups())
        if min(ta, tb) <= 0:
            raise SystemExit('Invalid zero turnaround time in captured data')
        fairness = min(ta, tb) / max(ta, tb)
        rows.append((runtime, trial, ta, tb, fairness))
        groups[runtime].append(fairness)
    if not rows:
        raise SystemExit('No FAIRNESS_DATA records found in the log')
    with args.output.with_suffix('.csv').open('w', newline='') as handle:
        writer = csv.writer(handle)
        writer.writerow(('runtime_ms', 'trial', 'turnaround_a_ms',
                         'turnaround_b_ms', 'fairness'))
        writer.writerows(rows)

    import matplotlib.pyplot as plt
    runtimes = sorted(groups)
    fig, ax = plt.subplots(figsize=(8, 4.5))
    ax.scatter([row[0] for row in rows], [row[4] for row in rows],
               alpha=0.4, label='Individual trials')
    ax.plot(runtimes, [mean(groups[r]) for r in runtimes], 'o-',
            label='Mean fairness')
    ax.axhline(1, color='gray', linestyle='--', label='Equal turnaround times')
    ax.set(xscale='log', ylim=(0, 1.05), xlabel='Requested CPU runtime per job (ms)',
           ylabel='F = min(turnaround) / max(turnaround)',
           title='Lottery scheduling fairness: two equal-ticket jobs')
    ax.grid(True, alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(args.output, dpi=200)
    print(f'Saved {args.output} and {args.output.with_suffix(".csv")} ({len(rows)} trials)')


if __name__ == '__main__':
    main()
