import FungibleToken from 0xf233dcee88fe0abe

transaction(path: String, receiverPath: String, amount: UFix64, recipient: Address) {
  let sentVault: @FungibleToken.Vault
  prepare(signer: AuthAccount) {
    let vaultRef = signer.borrow<&{FungibleToken.Vault}>(from: StoragePath(identifier: path)!)
      ?? panic("failed to borrow reference to sender vault")

    self.sentVault <- vaultRef.withdraw(amount: amount)
  }
  execute {
    let receiverRef = getAccount(recipient)
      .getCapability(PublicPath(identifier:receiverPath)!)
      .borrow<&{FungibleToken.Receiver}>()
        ?? panic("failed to borrow reference to recipient vault")

    receiverRef.deposit(from: <-self.sentVault)
  }
}