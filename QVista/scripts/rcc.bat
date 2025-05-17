@echo off
setlocal EnableDelayedExpansion

set "PROJECT_DIR=%~dp0..\"
set "QT_BIN=%PROJECT_DIR%..\External\qt\bin"
set "SRC_DIR=%PROJECT_DIR%"
set "OUT_DIR=%PROJECT_DIR%generated"

if not exist "%QT_BIN%\rcc.exe" (
    echo ERROR: rcc.exe not found in %QT_BIN%
    exit /b 1
)

if not exist "%OUT_DIR%" (
    mkdir "%OUT_DIR%"
)

for /R "%SRC_DIR%" %%f in (*.qrc) do (
    echo rcc: %%~nxf
    "%QT_BIN%\rcc.exe" "%%f" -o "%OUT_DIR%\qrc_%%~nf.cpp"
    if errorlevel 1 (
        echo ERROR running rcc on %%f
    )
)

endlocal
