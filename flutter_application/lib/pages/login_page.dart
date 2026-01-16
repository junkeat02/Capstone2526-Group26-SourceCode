import 'package:flutter/material.dart';
import 'package:flutter_application/pages/dashboard_page.dart';

class LoginPage extends StatefulWidget {
  const LoginPage({super.key});
  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  final _formKey = GlobalKey<FormState>();

  // Controllers
  final nameCtrl = TextEditingController();
  final ageCtrl = TextEditingController();
  final weightCtrl = TextEditingController();
  final heightCtrl = TextEditingController();
  final emergencyCtrl = TextEditingController();

  bool diabetic = false;
  String selectedGender = "Male";

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
            colors: [Color(0xFF0F2027), Color(0xFF203A43), Color(0xFF2C5364)],
          ),
        ),
        child: SafeArea(
          child: Center(
            child: SingleChildScrollView(
              padding: const EdgeInsets.all(24),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  _header(),
                  const SizedBox(height: 30),
                  _formCard(context),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }

  Widget _header() {
    return const Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          "Welcome to Fovian",
          style: TextStyle(
            fontSize: 28,
            fontWeight: FontWeight.bold,
            color: Colors.white,
          ),
        ),
        SizedBox(height: 6),
        Text(
          "Personalize your health monitoring",
          style: TextStyle(fontSize: 16, color: Colors.white70),
        ),
      ],
    );
  }

  Widget _formCard(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: Colors.black.withOpacity(0.35),
        borderRadius: BorderRadius.circular(24),
      ),
      child: Form(
        key: _formKey,
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            _inputField(
              nameCtrl,
              "Full Name",
              Icons.person,
              (v) => v!.isEmpty ? "Required" : null,
              isText: true,
            ),
            const SizedBox(height: 20),
            const Text(
              "Select Gender",
              style: TextStyle(color: Colors.white70, fontSize: 14),
            ),
            const SizedBox(height: 10),
            _genderSelection(),
            const SizedBox(height: 20),
            _inputField(
              ageCtrl,
              "Age",
              Icons.cake,
              (v) => num.tryParse(v!) == null ? "Invalid" : null,
            ),
            const SizedBox(height: 14),
            _inputField(
              weightCtrl,
              "Weight (kg)",
              Icons.monitor_weight,
              (v) => num.tryParse(v!) == null ? "Invalid" : null,
            ),
            const SizedBox(height: 14),
            _inputField(
              heightCtrl,
              "Height (m)",
              Icons.height,
              (v) => num.tryParse(v!) == null ? "Invalid" : null,
            ),
            const SizedBox(height: 14),
            _inputField(
              emergencyCtrl,
              "Emergency Contact",
              Icons.emergency,
              (v) => v!.length < 9 ? "Too short" : null,
              isPhone: true,
              prefix: "+60 ",
            ),
            const SizedBox(height: 16),
            _diabeticToggle(),
            const SizedBox(height: 24),
            _continueButton(context),
          ],
        ),
      ),
    );
  }

  Widget _genderSelection() {
    return Row(
      children: [
        Expanded(child: _genderTile("Male", Icons.male, Colors.blueAccent)),
        const SizedBox(width: 12),
        Expanded(child: _genderTile("Female", Icons.female, Colors.pinkAccent)),
      ],
    );
  }

  Widget _genderTile(String gender, IconData icon, Color color) {
    bool isSelected = selectedGender == gender;
    return GestureDetector(
      onTap: () => setState(() => selectedGender = gender),
      child: AnimatedContainer(
        duration: const Duration(milliseconds: 200),
        padding: const EdgeInsets.symmetric(vertical: 12),
        decoration: BoxDecoration(
          color: isSelected
              ? color.withOpacity(0.2)
              : Colors.black.withOpacity(0.3),
          borderRadius: BorderRadius.circular(16),
          border: Border.all(
            color: isSelected ? color : Colors.transparent,
            width: 2,
          ),
        ),
        child: Column(
          children: [
            Icon(icon, color: isSelected ? color : Colors.white54, size: 30),
            Text(
              gender,
              style: TextStyle(
                color: isSelected ? color : Colors.white54,
                fontWeight: isSelected ? FontWeight.bold : FontWeight.normal,
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _inputField(
    TextEditingController controller,
    String label,
    IconData icon,
    String? Function(String?)? validator, {
    bool isPhone = false,
    bool isText = false,
    String? prefix,
  }) {
    return TextFormField(
      controller: controller,
      keyboardType: isText
          ? TextInputType.text
          : (isPhone ? TextInputType.phone : TextInputType.number),
      style: const TextStyle(color: Colors.white),
      decoration: InputDecoration(
        prefixIcon: Icon(icon, color: Colors.tealAccent),
        prefixText: prefix,
        labelText: label,
        labelStyle: const TextStyle(color: Colors.white70),
        filled: true,
        fillColor: Colors.black.withOpacity(0.3),
        border: OutlineInputBorder(
          borderRadius: BorderRadius.circular(16),
          borderSide: BorderSide.none,
        ),
      ),
      validator: validator,
    );
  }

  Widget _diabeticToggle() {
    return SwitchListTile(
      value: diabetic,
      onChanged: (v) => setState(() => diabetic = v),
      title: const Text("Diabetic", style: TextStyle(color: Colors.white)),
      activeColor: Colors.tealAccent,
    );
  }

  Widget _continueButton(BuildContext context) {
    return SizedBox(
      width: double.infinity,
      height: 56,
      child: ElevatedButton(
        style: ElevatedButton.styleFrom(
          backgroundColor: Colors.tealAccent.shade700,
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(18),
          ),
        ),
        onPressed: () {
          if (_formKey.currentState!.validate()) {
            // 1. Get the raw text from the controller
            String rawInput = emergencyCtrl.text.trim();
            String finalContact;

            // 2. Check if the user already typed +60 or 60
            if (rawInput.startsWith('+60')) {
              finalContact = rawInput;
            } else if (rawInput.startsWith('60')) {
              finalContact = '+$rawInput';
            } else {
              // 3. If they just typed 123..., add the +60
              finalContact = "+60$rawInput";
            }

            Navigator.pushReplacement(
              context,
              MaterialPageRoute(
                builder: (_) => CombinedDashboard(
                  name: nameCtrl.text,
                  age: int.parse(ageCtrl.text),
                  weight: double.parse(weightCtrl.text),
                  height: double.parse(heightCtrl.text),
                  emergencyContact: finalContact, // Use the cleaned number here
                  isDiabetic: diabetic,
                  gender: selectedGender,
                ),
              ),
            );
          }
        },
        child: const Text(
          "Continue",
          style: TextStyle(color: Colors.black, fontWeight: FontWeight.bold),
        ),
      ),
    );
  }
}
