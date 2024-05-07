//
//  ContentView.swift
//  TheWorkoutBuddy_iOS
//
//  Created by David Lock on 5/6/24.
//

import SwiftUI

struct LoginView: View {

    @StateObject var viewModel = LoginView_ViewModel()

    var body: some View {
        NavigationView {
            VStack {
                // Bravo Builders & TWB header
                HeaderView()

                // Sign-in form
                Form {
                    TextField("Email Address", text: $viewModel.email)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                    SecureField("Password", text: $viewModel.password)
                        .textFieldStyle(RoundedBorderTextFieldStyle())

                    Button {
                        // Attempt
                    } label: {
                        ZStack {
                            RoundedRectangle(cornerRadius: 10)
                                .foregroundColor(Color.blue)

                            Text("Log In")
                                .foregroundColor(Color.white)
                                .bold()
                        }
                    }
                }

                // Create account
                VStack {
                    Text("Don't have an account?")

                    // Show registration
                    NavigationLink("Create an account",
                                    destination: RegisterView())
                }
                .padding(.bottom, 50)

                // Spacing
                Spacer()
            }
            .padding()
        }
    }
}

struct LoginView_Previews: PreviewProvider {
    static var previews: some View {
        LoginView()
    }
}
