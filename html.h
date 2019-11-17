const char changelog[] PROGMEM = R"=====(
<b>Device info</b>: Papa's Pegelzeiger<br>
<li>0.1: Batterieanzeige
<li>0.2: Umstellung auf Direktspannung, Updateserver hinzugef&uuml;gt
<li>0.3: Erweiterung um <b>/restart</b>, Optimierung der Strings, Versionsnummer im Display
<li>0.4: Umstellung auf Partialupdate, Anzeige der Solarerzeugung
<li>0.5: Verwendung von AsyncUdp f&uuml;r SMA
<li>0.6: Umstellung auf GxEPD2
<li>0.7: Pegel, Temperatur und Netto nun in eigenen Bereichen gezeichnet (nur wenn neue Werte vorliegen)
<li>0.8: Anzeige von Vorlauf- und R&uuml;cklauftemperatur, MedianFilter, Updatebenachrichtigung auf Bildschirm
<li>0.9: Screenupdate alle 30s, simple Filtermethode, die nur ca. 3% der Temperatur&auml;nderung bei jedem Lesevorgang ber&uuml;cksichtigt
<li>0.10: Versuchen, HttpClient resuable zu machen, keine Header mehr setzen
<li>0.11: Erfolgt mehr als 10Min keine Pegelaktualisierung, versuche HTTPClient wie 'read time reset' &uuml;ber NTPClient zu resetten
<li>0.12: now wird nun als nicht-lokale Variable gehalten und nur einmal pro loop()-Durchlauf gelesen
<li>0.12: weitere Optimierungen hinsichtlich Speicherverbrauch, NTP nicht bei jedem Loop (60s)
<li>0.13: Wurde NTP nicht initialisiert, dann wird dies nun sp&auml;ter nachgeholt; einige yields hinzugef&uuml;gt
<li>0.14: esp_task_wdt_init(999,false) in setup eingebaut, einige Timeouts geändert (nach 20s erstmals HTTP, Temp alle 1.5s, vorher 1.0s)
<li>0.15: Ausgabe der Versionsnummer beim Update
<li>0.16: Timeout von 999 auf 5000 erh&ouml;ht, statt yield nun vtaskdelay(10) in readPegel 
<li>0.17: Allgemeines ESP-Debuglogging abgeschaltet, 'enable esp debug'-Option f&uuml;r Kommandozeile 
<li>0.18: RunningSince eingebaut (inkl. print-Ausgabe)
<li>0.19: HTTP-Fehler im Klartext, POST statt GET
<li>0.20: Einige vTaskDelay(10); in OTA.cpp hinzu, direkte Verdrahtung von 192.168.178.24/data
<li>0.21: HttpClient lokal zu readPegel, kein reuse, mit end(), Zugriff wieder mit GET, POST crashed staendig
- TODO Ablage in GIT nachdem Wifi-Password in CIFFS liegt
)=====";

#define VERSION 0.21

const char update[] PROGMEM = R"=====(
<!DOCTYPE html><html lang="en" style="height:100%;"><head>
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
<title>Pegelanzeiger</title>
<style>
.c{text-align: center;} 
div,input{padding:5px;font-size:1em;} 
input{width:95%;margin-bottom:5px;border-radius: 4px;} 
body{
  height:100%;
  text-align: center;
  font-family:verdana;
  background-image:url("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHCAYAAADEUlfTAAAEfnpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjarVdttuMsCP7PKmYJAcSP5WiM57w7eJc/j8YmbW9vpp078SRYQEQeREvb//81+oVH2C3kLESfvF/wuOSSZHTisj875cWN7/6ESfmRT4dAwFJQ3X/6bepn8O0cENzkl0c+hXXaidPQbeZpUPvMgs7Ui9OQys7n+ZvSHJfd3XLmu27DxMLT6PNvFxCMamCqkGwKPr5jFoUHGjWDGr6sviuBZnVonZ9ex46O7lPwjt5T7JY8+foYClr8VPBPMZp8ttexGxG694jPmR8Ezg48v8SutRpb2/bVZecRKU9zUbeljB4UC0zpGObRAl5DP4yW0CKWuAKxCjQL2kqcWBDtxo4rZ268DbryChedbBJARVZEvPOiBkmyDlB6+B03CZq0EjASXYGagi2HLzzmTWO+lSNmrgxNYRhjjPjS6BXzb9phqLWeusxLPGIFv6QnINzoyPUvtAAItxlTG/Edje7yZrkDVoGgjTBHLDAvZTdRjM/c0oGzQs8WR8u+NTjUaQAhwtwGZ1iBwOJZjT0vQSQwI44R+GR4LuqkAAE2k8rUgI1iJwSJ0ufGmMBDV0x2NkoLgDD1GgBN0gywnDPkT3AROZRNzZGZeQsWLVn26p03733wvUbloMEFCz6EEEMKOWp00aKPIcaYYk6SFCXMkk+BUkwp5YxJM0xnjM7QyLlI0eKKFV9CiSWVvCJ9Vrfa6tewxjWtuUrViu1ffQ1UY001b7whlTa32ea3sMUtbbkh15o216z5FlpsqeUDtYnqI2r8hNw1ajxR64i5oRdO1MAO4WaCezmxjhkQE8dAPHQEkNDSMVsiOycduY7ZkgSbwgSosXVwKnfEgKDbWKzxgd2J3CVuZO4j3OQ75KhD9y+Qow7dRO4rbi9Qq3mcKDoA6ruwx3TRhsLWHCIRs5S41qV3YtdF2ssC95aDdU3pXUXQtGAdwq9V6IHxsR8npZ/5cVJCx3AGAjufModcNaO0leIq48WP3O8BVZ3Hu2X5TkiX0g+EdCEFuFFdy8iiHjVf2Vm/M9grSuggdQrG+9wsB+sXjC76kNKlQvX7FG+4RPwnlwoUkutl+bJD38nEcA6MjMdhv/sUTLnlCrxhP/WZuv7s0Jg1HjkRZHilNhPhtGVjQ3+bTXSRiYLKICmeoRgO6ZZiq74j+xxsqf/ALfrTBnnXLboh9FO36K368YZb3SM5ZmP/ZON9t+j9snbtFt3C9VO36KMyeOEWPaD4A7fo8+r82i36mlwXblkvE6h0MsvE/abFQYmzeR/kbqM/6CTpXQ/4T6N73HBW1w/8u5WxxwpZYLN8V5WvqBh9V8Ks9BKmuxduedqSp41cqk+4RWCLlNTdzLie4BgZ0QIoYGrop2KAeq2rj/1duDrTAMEXIV1KXwn7rXlTl0qseielKR5LYQhx5Uu6F+7rUD/dEyj+xYXhVWrSLezcU12PKJ4Fzg4cJu3g4lLp77fJXmofGO9RN3aYv8sc6ndO3Fxu2XQGRXHNw19z+g0EzfGFJDrJyAAAAAZiS0dEAP8A/wD/oL2nkwAAAAlwSFlzAAAOwwAADsMBx2+oZAAAAAd0SU1FB+IFEAwsCt9fsxIAAABDSURBVAjXfc67DYBADATRMUf/hVAGCU1ZHgInp+Oz0kZPtjau8zAiWKOyA4wxHpiZjQDztXY3PuMf0m/VZcyEVfWKN854GDbTdSsxAAAAAElFTkSuQmCC");} button{border:0;background-color:#b2ad98;color:#545147;line-height:2.4rem;font-size:1.2rem;width:100%;} #header{height:120px;text-align: left;color:#cbc6af;background-image:url("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAB3RJTUUH4gUQDDYwqX6QewAAAA1JREFUCNdjMDXWOwMAApoBY44cvvsAAAAASUVORK5CYII=");}#footer{height:24px;color:#292723;background-image:url("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAB3RJTUUH4gUQDDg61yhU6wAAAA1JREFUCNdjMDXWSwMAAjQA/b8+kbgAAAAASUVORK5CYII=");
}
#body{text-align:left;display:inline-block;min-width:260px;min-height: calc(100% - 186px);
color:#545147;}h2{margin-top: 4px;margin-left:10px;}p{margin-top: 2px;margin-bottom: 0px;margin-left: 10px;}
</style>
<script>var redirect = false;
{redirect};
if(redirect) {
  setTimeout('document.location.href="/update";', 3000);
}
</script>
</head>
<body>
<div id="header">
<h2>{banner}</h2>
<p>{build}</p><p>{branch}</p><p>{deviceInfo}</p></div> 
<div id="body"><form method='POST' action='' enctype='multipart/form-data'><input type='file' name='update'><br/>
<br/><input type='submit' value='Update'></form><br>{footer}</div><div id="footer"><p>Pegelmesser</p>
</div> </body></html>
)=====";

/**
 * You can disable diagnostic output in the .cpp file of each display class if needed by un-commenting:

Code: [Select]
//#define DISABLE_DIAGNOSTIC_OUTPUT
 */
