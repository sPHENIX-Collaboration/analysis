from bs4 import BeautifulSoup
import pandas as pd
import re
import requests
html = requests.get("https://www.cnipol.bnl.gov/fills/").text

soup = BeautifulSoup(html, "html.parser")

data = []
rows = soup.find_all("tr", class_="my_tr_highlight")

pattern = re.compile(r"([-+]?\d*\.?\d+)\s*±\s*([-+]?\d*\.?\d+)")

for row in rows:
    fill_id = row.find("a").text.strip()
    print(fill_id)

    blu_pol_values = row.find_all("td", class_="align_cm nowrap bluPol")
    yel_pol_values = row.find_all("td", class_="align_cm nowrap yelPol")

    blu_pol_match = pattern.match(blu_pol_values[0].text.strip()) if blu_pol_values else None
    yel_pol_match = pattern.match(yel_pol_values[0].text.strip()) if yel_pol_values else None

    blu_pol_mean = float(blu_pol_match.group(1)) if blu_pol_match else None
    blu_pol_error = float(blu_pol_match.group(2)) if blu_pol_match else None
    yel_pol_mean = float(yel_pol_match.group(1)) if yel_pol_match else None
    yel_pol_error = float(yel_pol_match.group(2)) if yel_pol_match else None

    data.append({
        "Fill ID": fill_id,
        "BluPol Mean": blu_pol_mean,
        "BluPol Error": blu_pol_error,
        "YelPol Mean": yel_pol_mean,
        "YelPol Error": yel_pol_error
    })

df = pd.DataFrame(data)
df = df.fillna(-999)
df.to_csv("updatedbeampolarizations.dat", sep=" ", index=False, header=False)

print(df)