@echo off
setlocal EnableDelayedExpansion

set "PROJECT_DIR=%~dp0..\"
set "QT_BIN=%PROJECT_DIR%..\External\qt\bin"
set "SRC_DIR=%PROJECT_DIR%"
set "OUT_DIR=%PROJECT_DIR%generated"

rem Confirm moc exists
if not exist "%QT_BIN%\moc.exe" (
    echo ERROR: moc.exe not found in %QT_BIN%
    exit /b 1
)

rem Create output dir if it doesn't exist
if not exist "%OUT_DIR%" (
    mkdir "%OUT_DIR%"
)

rem Loop over all .h files recursively
for /R "%SRC_DIR%" %%f in (*.h) do (
    findstr /C:"Q_OBJECT" "%%f" >nul
    if !errorlevel! == 0 (
        echo moc: %%~nxf
        "%QT_BIN%\moc.exe" "%%f" -o "%OUT_DIR%\moc_%%~nf.cpp"
        if errorlevel 1 (
            echo ERROR running moc on %%f
        )
    )
)

endlocal
