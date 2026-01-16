import 'package:flutter/material.dart';

class ProfileSettingsPage extends StatefulWidget {
  final Map<String, dynamic> currentData;

  const ProfileSettingsPage({super.key, required this.currentData});

  @override
  State<ProfileSettingsPage> createState() => _ProfileSettingsPageState();
}

class _ProfileSettingsPageState extends State<ProfileSettingsPage> {
  final _formKey = GlobalKey<FormState>();

  late TextEditingController ageCtrl;
  late TextEditingController weightCtrl;
  late TextEditingController heightCtrl;
  late TextEditingController emergencyCtrl;
  late bool diabetic;
  late String selectedGender;

  @override
  void initState() {
    super.initState();
    // Use null-aware operators (??) to prevent crashes (black screens)
    ageCtrl = TextEditingController(text: (widget.currentData['age'] ?? '').toString());
    weightCtrl = TextEditingController(text: (widget.currentData['weight'] ?? '').toString());
    heightCtrl = TextEditingController(text: (widget.currentData['height'] ?? '').toString());
    emergencyCtrl = TextEditingController(text: (widget.currentData['emergency'] ?? '').toString());
    diabetic = widget.currentData['diabetic'] ?? false;
    selectedGender = widget.currentData['gender'] ?? "Male";
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF0A0E12),
      appBar: AppBar(
        backgroundColor: Colors.transparent,
        elevation: 0,
        title: const Text("PROFILE SETTINGS",
            style: TextStyle(fontWeight: FontWeight.bold, letterSpacing: 1.2)),
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(24),
        child: Form(
          key: _formKey,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              _sectionHeader("Personal Details"),
              const SizedBox(height: 16),
              _buildGenderPicker(),
              const SizedBox(height: 30),
              _sectionHeader("Physical Information"),
              const SizedBox(height: 16),
              _buildEditField(ageCtrl, "Age", Icons.cake, (v) {
                if (v == null || v.isEmpty) return "Required";
                if (num.tryParse(v) == null) return "Invalid number";
                return null;
              }),
              const SizedBox(height: 16),
              _buildEditField(weightCtrl, "Weight (kg)", Icons.monitor_weight, (v) {
                if (v == null || v.isEmpty) return "Required";
                return null;
              }),
              const SizedBox(height: 16),
              _buildEditField(heightCtrl, "Height (m)", Icons.height, (v) {
                if (v == null || v.isEmpty) return "Required";
                return null;
              }),
              const SizedBox(height: 30),
              _sectionHeader("Emergency Contact"),
              const SizedBox(height: 16),
              _buildEditField(emergencyCtrl, "Phone Number", Icons.emergency, (v) {
                if (v == null || v.isEmpty) return "Required";
                return null;
              }, prefix: "+60 "),
              const SizedBox(height: 20),
              _buildToggle(),
              const SizedBox(height: 40),
              _saveButton(), // <--- This fixes the "unused_element" error
            ],
          ),
        ),
      ),
    );
  }

  // --- HELPER WIDGETS ---

  Widget _sectionHeader(String title) {
    return Text(title.toUpperCase(),
        style: const TextStyle(color: Colors.tealAccent, fontWeight: FontWeight.bold, fontSize: 14));
  }

  Widget _buildGenderPicker() {
    return Row(
      children: [
        _genderIconOption("Male", Icons.male, Colors.blueAccent),
        const SizedBox(width: 20),
        _genderIconOption("Female", Icons.female, Colors.pinkAccent),
      ],
    );
  }

  Widget _genderIconOption(String gender, IconData icon, Color color) {
    bool isSelected = selectedGender == gender;
    return GestureDetector(
      onTap: () => setState(() => selectedGender = gender),
      child: Column(
        children: [
          CircleAvatar(
            radius: 30,
            backgroundColor: isSelected ? color : const Color(0xFF15191E),
            child: Icon(icon, color: isSelected ? Colors.white : Colors.white54, size: 30),
          ),
          const SizedBox(height: 8),
          Text(gender, style: TextStyle(color: isSelected ? color : Colors.white54, fontSize: 12)),
        ],
      ),
    );
  }

  Widget _buildEditField(TextEditingController ctrl, String label, IconData icon, String? Function(String?)? validator, {String? prefix}) {
    return TextFormField(
      controller: ctrl,
      keyboardType: const TextInputType.numberWithOptions(decimal: true),
      style: const TextStyle(color: Colors.white),
      decoration: InputDecoration(
        labelText: label,
        labelStyle: const TextStyle(color: Colors.white54),
        prefixIcon: Icon(icon, color: Colors.tealAccent),
        prefixText: prefix,
        filled: true,
        fillColor: const Color(0xFF15191E),
        border: OutlineInputBorder(borderRadius: BorderRadius.circular(16), borderSide: BorderSide.none),
      ),
      validator: validator,
    );
  }

  Widget _buildToggle() {
    return Container(
      decoration: BoxDecoration(color: const Color(0xFF15191E), borderRadius: BorderRadius.circular(16)),
      child: SwitchListTile(
        title: const Text("Diabetic Condition", style: TextStyle(color: Colors.white)),
        value: diabetic,
        activeColor: Colors.tealAccent,
        onChanged: (v) => setState(() => diabetic = v),
      ),
    );
  }

  Widget _saveButton() {
    return SizedBox(
      width: double.infinity,
      height: 60,
      child: ElevatedButton(
        style: ElevatedButton.styleFrom(
          backgroundColor: Colors.tealAccent.shade700,
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
        ),
        onPressed: () {
          if (_formKey.currentState!.validate()) {
            Navigator.pop(context, {
              'age': int.tryParse(ageCtrl.text) ?? 0,
              'weight': double.tryParse(weightCtrl.text) ?? 0.0,
              'height': double.tryParse(heightCtrl.text) ?? 0.0,
              'emergency': emergencyCtrl.text,
              'diabetic': diabetic,
              'gender': selectedGender,
            });
          }
        },
        child: const Text("UPDATE PROFILE",
            style: TextStyle(color: Colors.black, fontWeight: FontWeight.bold, fontSize: 18)),
      ),
    );
  }
}