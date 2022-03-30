use std::convert::TryFrom;
use filecoin_signer::{transaction_sign_raw, PrivateKey};

#[cxx::bridge(namespace = bls)]
mod ffi {
    extern "Rust" {
        fn fil_private_key_public_key(private_key: &[u8; 32]) -> [u8; 48];
        fn transaction_sign(
          transaction: &str,
          private_key:  &[u8; 32],
      ) -> String;
    }
}

fn transaction_sign(
  transaction: &str,
  private_key_bytes: &[u8; 32]
) -> String {
  
  let raw_signature = transaction_sign(transaction, &private_key_bytes);
  println!("{:?}", raw_signature);
  
  return raw_signature;
}

/// Generates a public key from the private key
/// Original implementation in Filecoin xFFI project:
/// https://github.com/filecoin-project/filecoin-ffi/blob/master/rust/src/bls/api.rs#L324
pub fn fil_private_key_public_key(private_key: &[u8; 32]) -> [u8; 48] {
    let mut public_key: [u8; 48] = [0; 48];
    /*
    let wrapped_private_key = PrivateKey::from_bytes(private_key);
    if let Ok(wrapped_private_key) = wrapped_private_key {
        wrapped_private_key
            .public_key()
            .write_bytes(&mut public_key.as_mut())
            .expect("preallocated");
    }*/
    return public_key;
}
