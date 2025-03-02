@echo off
echo.
echo === Начинаем компиляцию проекта ===
echo.

:: Указываем путь к файлу решения
set SOLUTION_FILE="spankerfield.sln"

:: Указываем конфигурацию компиляции
set CONFIGURATION=x64
set PLATFORM=Release

:: Вызываем MSBuild для компиляции
echo Компилируем %SOLUTION_FILE% в конфигурации %CONFIGURATION% %PLATFORM%...
msbuild %SOLUTION_FILE% /p:Configuration=%PLATFORM% /p:Platform=%CONFIGURATION%

:: Проверяем результат компиляции
if %errorlevel% neq 0 (
    echo.
    echo === Компиляция завершилась с ошибками ===
    exit /b 1
) else (
    echo.
    echo === Компиляция успешно завершена ===
)

pause