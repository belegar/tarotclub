@echo off

REM Delete old file
rm -f %CD%\report.txt

perl "vera++\generate_list.pl" %CD%\..
"C:\Program Files (x86)\vera++\bin\vera++.exe" -r %CD%\vera++ -s -i %CD%\files.txt -o %CD%\report.txt -P max-line-length=150
