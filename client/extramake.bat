:: copy license file to build directory
xcopy /Q /Y "%1\..\..\LICENSE" "%2\debug\"
xcopy /Q /Y "%1\..\..\LICENSE" "%2\release\"
