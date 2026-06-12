#!/usr/bin/env python3
"""
Сравнение указанного файла со всеми другими файлами, имеющими такой же суффикс,
с удалением совпадающих файлов.
"""

import os
import sys
import argparse
import filecmp
from pathlib import Path


def parse_arguments():
    """Разбор аргументов командной строки"""
    parser = argparse.ArgumentParser(
        prog='solution.py',
        description='Сравнение указанного файла со всеми другими файлами, '
                    'имеющими такой же суффикс, с удалением совпадающих файлов.',
        usage='%(prog)s [OPTIONS] <filename> <suffix>',
        epilog='Примеры:\n'
               '  %(prog)s file.txt .txt          # Сравнить file.txt с другими .txt файлами\n'
               '  %(prog)s -x document.doc .doc   # С удалением совпадающих файлов с трассировкой'
    )
    
    parser.add_argument('filename', nargs='?', help='Исходный файл для сравнения')
    parser.add_argument('suffix', nargs='?', help='Суффикс файлов для поиска (например: .txt)')
    parser.add_argument('-x', '--trace', action='store_true', help='Включить режим трассировки')
    
    return parser.parse_args()


def prompt_params(filename, suffix):
    """Запрос параметров, если они не указаны"""
    if not filename:
        filename = input("Введите имя исходного файла: ").strip()
    
    if not suffix:
        suffix = input("Введите суффикс файлов (например: .txt): ").strip()
    
    return filename, suffix


def main():
    args = parse_arguments()
    
    # Запрос параметров при необходимости
    filename, suffix = args.filename, args.suffix
    if not filename or not suffix:
        filename, suffix = prompt_params(filename, suffix)
    
    # Включение режима трассировки
    if args.trace:
        import trace
        tracer = trace.Trace(trace=1, count=0)
        tracer.runfunc(process_files, filename, suffix, args.trace)
    else:
        process_files(filename, suffix, args.trace)


def process_files(filename, suffix, trace_mode=False):
    """Основная логика обработки файлов"""
    # Проверка существования исходного файла
    if not os.path.exists(filename):
        print(f"Ошибка: Файл '{filename}' не существует")
        sys.exit(1)
    
    if not os.path.isfile(filename):
        print(f"Ошибка: '{filename}' не является файлом")
        sys.exit(1)
    
    # Нормализация суффикса
    if suffix and not suffix.startswith('.'):
        print(f"Предупреждение: Суффикс '{suffix}' не начинается с точки. Добавляем точку...")
        suffix = '.' + suffix
    
    print(f"Поиск файлов с суффиксом '{suffix}' в текущей директории...")
    
    # Получение директории исходного файла
    file_dir = os.path.dirname(filename)
    if not file_dir:
        file_dir = '.'
    
    # Поиск всех файлов с данным суффиксом
    pattern = f"*{suffix}"
    matching_files = Path(file_dir).glob(pattern)
    
    deleted_count = 0
    
    for file_path in matching_files:
        file_str = str(file_path)
        
        # Пропускаем сам исходный файл
        if os.path.samefile(file_str, filename):
            continue
        
        # Сравнение файлов
        if filecmp.cmp(filename, file_str, shallow=False):
            print(f"Файл '{file_str}' идентичен '{filename}'")
            
            # Удаление файла
            try:
                os.remove(file_str)
                print(f"  -> Удалён: {file_str}")
                deleted_count += 1
            except OSError as e:
                print(f"  -> Ошибка при удалении: {file_str} - {e}")
    
    if deleted_count == 0:
        print("Совпадающих файлов не найдено.")
    else:
        print(f"Удалено файлов: {deleted_count}")
    
    return deleted_count


if __name__ == "__main__":
    main()
