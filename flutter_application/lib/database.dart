import 'package:sqflite/sqflite.dart';
import 'package:path/path.dart';

class HealthDatabase {
  static final HealthDatabase instance = HealthDatabase._init();
  static Database? _database;

  HealthDatabase._init();

  Future<Database> get database async {
    if (_database != null) return _database!;
    _database = await _initDB('health_monitoring.db');
    return _database!;
  }

  Future<Database> _initDB(String filePath) async {
    final dbPath = await getDatabasesPath();
    final path = join(dbPath, filePath);

    return await openDatabase(
      path,
      version: 1,
      onCreate: _createDB,
    );
  }

  Future _createDB(Database db, int version) async {
    await db.execute('''
      CREATE TABLE health_logs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        heartRate REAL,
        spo2 INTEGER,
        bloodSugar REAL,
        steps INTEGER,
        timestamp TEXT
      )
    ''');
  }

  // INSERT LOG
  Future<void> insertLog(double hr, int spo2, double bs, int steps) async {
    final db = await instance.database;
    await db.insert('health_logs', {
      'heartRate': hr,
      'spo2': spo2,
      'bloodSugar': bs,
      'steps': steps,
      // Storing as ISO8601 string for easy SQLite date filtering
      'timestamp': DateTime.now().toIso8601String(),
    });
    print("DB: Saved entry - HR: $hr, SPO2: $spo2");
  }

  // FILTERED FETCH
  Future<List<Map<String, dynamic>>> getLogsFiltered(String period) async {
    final db = await instance.database;
    String timeModifier;

    switch (period) {
      case '1h':
        timeModifier = '-1 hour';
        break;
      case '24h':
        timeModifier = '-1 day';
        break;
      case '7d':
        timeModifier = '-7 days';
        break;
      default:
        timeModifier = '-1 hour';
    }

    // This query selects data where the timestamp is newer than the modifier
    // 'now' and 'localtime' ensure it matches your phone's current time
    return await db.query(
      'health_logs',
      where: "timestamp >= datetime('now', ?, 'localtime')",
      whereArgs: [timeModifier],
      orderBy: "timestamp ASC",
    );
  }

  // CLEAR ALL (For testing purposes)
  Future<void> clearDatabase() async {
    final db = await instance.database;
    await db.delete('health_logs');
  }
}