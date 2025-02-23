#!/usr/bin/python3

import psycopg2
import cgi

print("Content-type:text/html\r\n\r\n")
print('<html>')
print('<head>')
print('<title>EMCAL vop</title>')
print('</head>')
print('<body>')
print('<h2>EMCAL V<sub>op</sub></h2>')

print("<form action=\"vop.py\" method=\"post\">")
print("<table>")
print("<tr>")
print("<td>Sector </td>")
print("<td><input type=\"text\" name=\"sector\" value=1></td>")
print("</tr><tr>")
print("<td>V<sub>MPOD</sub></td>")
print("<td><input type=\"text\" name=\"vmpod\" value=66.5></td>")
print("</tr><tr>")
print("<td><input type=\"submit\" value=\"Submit\" /></td>")
print("</tr>")
print("</table>")
print("</form>")


try:
    conn = psycopg2.connect(host="sphnxdbmaster.sdcc.bnl.gov", database="emcal", user="phnxrc", password="")
    cur = conn.cursor()
#    sql_select = "select * from vop "
#    sql_select = "select *,cast(1000*(bias-69.0) as int) as gs from vop "

    form = cgi.FieldStorage()
    vmpod = form.getvalue("vmpod","66.5")
    sql_select = "select *,cast(1000*(bias-"+vmpod+"-2.5) as int) as gs from vop "
#   sql_where = form['sql_where'].value
    sql_where = "where sector="
    sql_where += form.getvalue("sector","1")
    sql_order = " order by sector,ib,channel asc;"

    sql = sql_select + sql_where + sql_order
        
    cur.execute(sql)

    print(sql+"<br><hr>\n")
    print("Number of rows: ", cur.rowcount)
    print('<table border=1>')
    col_names = [cn[0] for cn in cur.description]
    print("<tr>")
    for xn in col_names:
        print("<th>"+xn+"</th>")
    print("</tr>")  

    row = cur.fetchone()
    while row is not None:
#        print(row)
        print('<tr>')
        for x in row:
            print('<td>'+str(x)+'</td>')
        print('</tr>')
        row = cur.fetchone()
    print('</table>')
    
    cur.close()
except (Exception, psycopg2.DatabaseError) as error:
    print(error)
finally:
    if conn is not None:
        conn.close()

print('</body>')
print('</html>')

