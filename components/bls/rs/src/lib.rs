use bls_signatures::{PrivateKey, Serialize};
pub mod signature;
use crate::signature::{
  UnsignedMessageAPI
};
use crate::signature::transaction_sign_raw;

#[cxx::bridge(namespace = bls)]
mod ffi {
    extern "Rust" {
        fn fil_private_key_public_key(private_key: &[u8; 32]) -> [u8; 48];
        fn transaction_sign(
          transaction: &str,
          private_key: &str,
      ) -> String;
    }
}

fn transaction_sign(
  transaction: &str,
  private_key_base64: &str,
) -> String {
  let message_user_api: UnsignedMessageAPI = serde_json::from_str(transaction).unwrap();
  let private_key = PrivateKey::try_from(*private_key_base64).unwrap();
  let raw_signature = transaction_sign_raw(&message_user_api, &private_key).unwrap();
  println!("{:?}", raw_signature);
  return "".to_string();
}

/// Generates a public key from the private key
/// Original implementation in Filecoin xFFI project:
/// https://github.com/filecoin-project/filecoin-ffi/blob/master/rust/src/bls/api.rs#L324
pub fn fil_private_key_public_key(private_key: &[u8; 32]) -> [u8; 48] {
    let wrapped_private_key = PrivateKey::from_bytes(private_key);
    let mut public_key: [u8; 48] = [0; 48];
    if let Ok(wrapped_private_key) = wrapped_private_key {
        wrapped_private_key
            .public_key()
            .write_bytes(&mut public_key.as_mut())
            .expect("preallocated");
    }
    return public_key;
}
