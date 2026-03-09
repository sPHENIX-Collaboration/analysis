#!/usr/bin/env python3

import pyodbc

def get_nEvents(cursor, runlist):
    with open(runlist, 'r') as f:
        runstrings = f.readlines()
    runnumbers = [int(run) for run in runstrings]
    nEvents = []
    for runnumber in runnumbers:
        query = f"""
        SELECT SUM(events)
        FROM datasets
        WHERE runnumber = {runnumber} AND filename LIKE 'DST_CALO_run2pp_ana437_2024p007-%';
        """
        cursor.execute(query)
        events = [row.sum for row in cursor.fetchall()]
        if len(events) != 1:
            print(f'Found {len(events)} rows in cursor.fetchall()')
        else:
            nEvents.append(events[0])
    print(f'Found {len(nEvents)} runs in file {runlist}')
    print(f'Found {sum(nEvents)} total events in file {runlist}')
    return sum(nEvents)


def get_all_run_numbers(cursor):
    query = """
    SELECT runnumber, SUM(events)
    FROM datasets
    WHERE filename like 'DST_CALO_run2pp_ana437_2024p007-%'
    GROUP BY runnumber
    HAVING SUM(events) >= 500000;
    """
    cursor.execute(query)
    # run_numbers = [row.runnumber for row in cursor.fetchall()]
    run_numbers = []
    nEvents = []
    for row in cursor.fetchall():
        run_numbers.append(row.runnumber)
        nEvents.append(row.sum)
    print(f'Total events in all p007 runs with >= 500k events: {sum(nEvents)}')
    return run_numbers


def get_all_run_numbers_no_event_count(cursor):
    query = """
    SELECT runnumber, SUM(events)
    FROM datasets
    WHERE filename like 'DST_CALO_run2pp_ana437_2024p007-%'
    GROUP BY runnumber;
    """
    cursor.execute(query)
    # all_runs_no_event_count = [row.runnumber for row in cursor.fetchall()]
    # all_runs_nEvents = [row.sum for row in cursor.fetchall()]
    all_runs_no_event_count = []
    all_runs_nEvents = []
    for row in cursor.fetchall():
        all_runs_no_event_count.append(row.runnumber)
        all_runs_nEvents.append(row.sum)
    print(f'Total events in all p007 runs: {sum(all_runs_nEvents)}')
    return all_runs_no_event_count


def get_good_run_numbers(production_cursor):
    query = """
    SELECT runnumber
    FROM goodruns
    """
    production_cursor.execute(query)
    good_run_numbers = {row.runnumber for row in production_cursor.fetchall()}
    return list(good_run_numbers)

def get_emcal_auto_golden_run_numbers(file_catalog_run_numbers, production_cursor):
    query = """
    SELECT runnumber
    FROM goodruns
    WHERE (emcal_auto).runclass = 'GOLDEN'
    """
    production_cursor.execute(query)
    emcal_auto_golden_run_numbers = {row.runnumber for row in production_cursor.fetchall()}
    return list(
        emcal_auto_golden_run_numbers.intersection(
            set(file_catalog_run_numbers)
        )
    )

def get_ihcal_auto_golden_run_numbers(file_catalog_run_numbers, production_cursor):
    query = """
    SELECT runnumber
    FROM goodruns
    WHERE (ihcal_auto).runclass = 'GOLDEN'
    """
    production_cursor.execute(query)
    ihcal_auto_golden_run_numbers = {row.runnumber for row in production_cursor.fetchall()}
    return list(
        ihcal_auto_golden_run_numbers.intersection(
            set(file_catalog_run_numbers)
        )
    )

def get_ohcal_auto_golden_run_numbers(file_catalog_run_numbers, production_cursor):
    query = """
    SELECT runnumber
    FROM goodruns
    WHERE (ohcal_auto).runclass = 'GOLDEN'
    """
    production_cursor.execute(query)
    ohcal_auto_golden_run_numbers = {row.runnumber for row in production_cursor.fetchall()}
    return list(
        ohcal_auto_golden_run_numbers.intersection(
            set(file_catalog_run_numbers)
        )
    )

def main():
    # Connect to the FileCatalog database
    file_catalog_conn = pyodbc.connect("DSN=FileCatalog;UID=phnxrc;READONLY=True")
    file_catalog_cursor = file_catalog_conn.cursor()

    all_runs_no_event_count = get_all_run_numbers_no_event_count(file_catalog_cursor)
    with open('runNumbers_all_p007.txt', 'w') as f:
        for run_number in all_runs_no_event_count:
            f.write(f"{run_number}\n")
    print(f"Total number of p007 runs (regardless of event count): {len(all_runs_no_event_count)}")
    # nEvents_all_no_event_count = get_nEvents(file_catalog_cursor, 'runNumbers_all_p007.txt')
    # print(f'Total number of p007 events: {nEvents_all_no_event_count}')


    # Get unique run numbers with at least 500k total events
    file_catalog_run_numbers = get_all_run_numbers(file_catalog_cursor)
    file_catalog_run_numbers.sort()
    with open('runNumbers_with_AtLeast_500kEvents.txt', 'w') as f:
        for run_number in file_catalog_run_numbers:
            f.write(f"{run_number}\n")
    print(f"Number of all runs saved to runNumbers_with_AtLeast_500kEvents.txt that have at least 500k events: {len(file_catalog_run_numbers)}")

    # Connect to the Production database
    production_conn = pyodbc.connect("DSN=Production_write")
    production_cursor = production_conn.cursor()

    # Filter good run numbers
    good_run_numbers = get_good_run_numbers(production_cursor)
    runs_not_in_goodruns = set(file_catalog_run_numbers) - set(good_run_numbers)
    print(f"Number of runs not in the goodruns table: {len(runs_not_in_goodruns)}")

    # Filter 'GOLDEN' run numbers
    emcal_auto_golden_run_numbers = get_emcal_auto_golden_run_numbers(file_catalog_run_numbers, production_cursor)
    emcal_auto_golden_run_numbers.sort()
    with open('GoldenEmcalRunList.txt', 'w') as f:
        for run_number in emcal_auto_golden_run_numbers:
            f.write(f"{run_number}\n")
    print(f"Number of EMCal GOLDEN runs saved to GoldenEmcalRunList.txt: {len(emcal_auto_golden_run_numbers)}")
    nEventsEMCal = get_nEvents(file_catalog_cursor, 'GoldenEmcalRunList.txt')

    ihcal_auto_golden_run_numbers = get_ihcal_auto_golden_run_numbers(file_catalog_run_numbers, production_cursor)
    ihcal_auto_golden_run_numbers.sort()
    with open('GoldenIHCalRunList.txt', 'w') as f:
        for run_number in ihcal_auto_golden_run_numbers:
            f.write(f"{run_number}\n")
    print(f"Number of IHCal GOLDEN runs saved to GoldenIHCalRunList.txt: {len(ihcal_auto_golden_run_numbers)}")

    ohcal_auto_golden_run_numbers = get_ohcal_auto_golden_run_numbers(file_catalog_run_numbers, production_cursor)
    ohcal_auto_golden_run_numbers.sort()
    with open('GoldenOHCalRunList.txt', 'w') as f:
        for run_number in ohcal_auto_golden_run_numbers:
            f.write(f"{run_number}\n")
    print(f"Number of OHCal GOLDEN runs saved to GoldenOHCalRunList.txt: {len(ohcal_auto_golden_run_numbers)}")

    # Get the intersection of all sets
    golden_run_numbers = emcal_auto_golden_run_numbers
    golden_run_numbers.sort()
    with open('GoldenCalorimeterRunList.txt', 'w') as f:
        for run_number in golden_run_numbers:
            f.write(f"{run_number}\n")
    print(f"Number of Calo GOLDEN runs saved to GoldenCalorimeterRunList.txt: {len(golden_run_numbers)}")

    # Close the Production database connection
    production_conn.close()

    nEvents_GL1 = get_nEvents(file_catalog_cursor, 'GoldenGL1RunList.txt')
    nEvents_FEM = get_nEvents(file_catalog_cursor, 'GoldenFEMrunList.txt')
    nEvents_withmaps = get_nEvents(file_catalog_cursor, '../RD/2024/Nov7/emcal_withmaps.txt')
    nEvents_validspin = get_nEvents(file_catalog_cursor, '../RD/2024/Nov7/validspinruns.txt')

    # Close the FileCatalog database connection
    file_catalog_conn.close()

if __name__ == "__main__":
    main()


