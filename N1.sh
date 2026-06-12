#!/bin/bash

show_help() {
    cat << EOF
Usage: $0 [OPTIONS] <filename> <suffix>

Сравнение указанного файла со всеми другими файлами, имеющими такой же суффикс,
с удалением совпадающих файлов.

Arguments:
  filename    Исходный файл для сравнения
  suffix      Суффикс файлов для поиска (например: .txt, .log)

Options:
  -h, --help  Показать эту справку
  -x, --trace Включить режим трассировки

Examples:
  $0 file.txt .txt          # Сравнить file.txt с другими .txt файлами
  $0 -x document.doc .doc   # С удалением совпадающих файлов с трассировкой
EOF
}

prompt_params() {
    if [ -z "$1" ]; then
        read -p "Введите имя исходного файла: " filename
    else
        filename="$1"
    fi
    
    if [ -z "$2" ]; then
        read -p "Введите суффикс файлов (например: .txt): " suffix
    else
        suffix="$2"
    fi
}

trace_mode=0
filename=""
suffix=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -x|--trace)
            trace_mode=1
            shift
            ;;
        -*)
            echo "Ошибка: Неизвестная опция $1"
            show_help
            exit 1
            ;;
        *)
            if [ -z "$filename" ]; then
                filename="$1"
            elif [ -z "$suffix" ]; then
                suffix="$1"
            else
                echo "Ошибка: Слишком много аргументов"
                show_help
                exit 1
            fi
            shift
            ;;
    esac
done

if [ $trace_mode -eq 1 ]; then
    set -x
fi

# Запрос параметров, если они не указаны
if [ -z "$filename" ] || [ -z "$suffix" ]; then
    prompt_params "$filename" "$suffix"
fi

if [ ! -f "$filename" ]; then
    echo "Ошибка: Файл '$filename' не существует"
    exit 1
fi

if [[ ! "$suffix" =~ ^\..* ]] && [ -n "$suffix" ]; then
    echo "Предупреждение: Суффикс '$suffix' не начинается с точки. Добавляем точку..."
    suffix=".$suffix"
fi

echo "Поиск файлов с суффиксом '$suffix' в текущей директории..."

# Получение директории исходного файла
file_dir=$(dirname "$filename")
file_basename=$(basename "$filename" "$suffix")

# Поиск всех файлов с данным суффиксом
shopt -s nullglob
files=("$file_dir"/*"$suffix")

deleted_count=0
matched_files=()

# Сравнение файлов
for file in "${files[@]}"; do
    # Пропускаем сам исходный файл
    if [ "$file" = "$filename" ]; then
        continue
    fi
    
    if cmp -s "$filename" "$file"; then
        echo "Файл '$file' идентичен '$filename'"
        matched_files+=("$file")
        
        # Удаление файла
        if rm "$file"; then
            echo "  -> Удалён: $file"
            ((deleted_count++))
        else
            echo "  -> Ошибка при удалении: $file"
        fi
    fi
done

if [ $deleted_count -eq 0 ]; then
    echo "Совпадающих файлов не найдено."
else
    echo "Удалено файлов: $deleted_count"
fi

# Отключение трассировки
if [ $trace_mode -eq 1 ]; then
    set +x
fi

exit 0
