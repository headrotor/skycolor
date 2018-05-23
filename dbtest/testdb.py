import MySQLdb 
import subprocess 
import re 
import sys 
import time 
import datetime 
try:
    from ConfigParser import ConfigParser
except ImportError:
    from configparser import ConfigParser

cfg = ConfigParser()
#cfg.read('default.cfg')
cfg.read('dbcred.cfg')

dbhost = cfg.get('DB-cred','dbhost')
print(dbhost)
dbuser = cfg.get('DB-cred','dbuser')
dbpass = cfg.get('DB-cred','dbpass')
dbname = cfg.get('DB-cred','dbname')


# Open database connection 
dbconn = MySQLdb.connect(dbhost,dbuser,dbpass,dbname) or die("could not connect to database") 
cursor=dbconn.cursor() 
# Continuously append data 

timestamp = datetime.datetime.now() 
today = timestamp.strftime("%d/%m/%Y %H:%M:%S") 
print(today )
# Run the DHT program to get the humidity and temperature readings! 

# MYSQL DATA Processing 
print("injecting sql!" )

cursor.execute("INSERT INTO test (datetime, testfdata, testidata ) VALUES (%s, %s, %s)", (timestamp, 80, 666.6)) 
dbconn.commit() 

cursor.close
